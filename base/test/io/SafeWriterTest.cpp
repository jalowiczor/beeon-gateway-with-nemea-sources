#include <sstream>

#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Path.h>

#include "cppunit/BetterAssert.h"
#include "cppunit/FileTestFixture.h"
#include "io/SafeWriter.h"
#include "util/PosixSignal.h"

using namespace Poco;
using namespace std;

namespace BeeeOn {

class SafeWriterTest : public FileTestFixture {
	CPPUNIT_TEST_SUITE(SafeWriterTest);
	CPPUNIT_TEST(testWriteAtomic);
	CPPUNIT_TEST(testRollback);
	CPPUNIT_TEST(testTmpExists);
	CPPUNIT_TEST(testConflictingWriters);
	CPPUNIT_TEST(testFinalize);
	CPPUNIT_TEST(testTooBig);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	void tearDown();
	void testWriteAtomic();
	void testRollback();
	void testTmpExists();
	void testConflictingWriters();
	void testFinalize();
	void testTooBig();

protected:
	File testingTmpFile() const;
};

CPPUNIT_TEST_SUITE_REGISTRATION(SafeWriterTest);

void SafeWriterTest::setUp()
{
	FileTestFixture::setUp();

	try {
		testingTmpFile().remove();
	}
	catch (...) {}
}

void SafeWriterTest::tearDown()
{
	try {
		testingTmpFile().remove();
	}
	catch (...) {}

	FileTestFixture::tearDown();
}

/**
 * @return path to the lock file derived from FileTestFixture::testingPath().
 */
File SafeWriterTest::testingTmpFile() const
{
	auto path = testingPath();
	auto ext = path.getExtension();

	if (!ext.empty())
		ext += ".";

	path.setExtension(ext + "lock");
	return path;
}

/**
 * @brief Test that writing to the testingFile() is atomic. We either get
 * a fully and successfully written file or nothing. The intermediate tmp
 * file can however be visible and contain just partial data.
 */
void SafeWriterTest::testWriteAtomic()
{
	CPPUNIT_ASSERT_FILE_TEXTUAL_EQUALS("", testingFile());
	CPPUNIT_ASSERT(!testingTmpFile().exists());

	SafeWriter writer(testingTmpFile());
	CPPUNIT_ASSERT(!testingTmpFile().exists());

	ostream &out = writer.stream();
	CPPUNIT_ASSERT(testingTmpFile().exists());
	CPPUNIT_ASSERT(out.good());

	out << "partial content";
	CPPUNIT_ASSERT(out.good());
	writer.flush();
	CPPUNIT_ASSERT(out.good());

	CPPUNIT_ASSERT_FILE_TEXTUAL_EQUALS("", testingFile());
	CPPUNIT_ASSERT_FILE_TEXTUAL_EQUALS("partial content", testingTmpFile());

	out << " completed";
	CPPUNIT_ASSERT(out.good());
	writer.commitAs(testingFile());

	CPPUNIT_ASSERT(!testingTmpFile().exists());
	CPPUNIT_ASSERT_FILE_TEXTUAL_EQUALS("partial content completed", testingFile());
}

/**
 * @brief Test we can rollback writing the current file regardless of any I/O errors.
 * The target file is untouched and contains the original data until we call commitAs()
 * which never happens in this test. Thus, nobody can see any changes the target file.
 */
void SafeWriterTest::testRollback()
{
	writeFile(testingFile(), "some data");
	CPPUNIT_ASSERT_FILE_TEXTUAL_EQUALS("some data", testingFile());
	CPPUNIT_ASSERT(!testingTmpFile().exists());

	SafeWriter writer(testingTmpFile());
	CPPUNIT_ASSERT(!testingTmpFile().exists());

	ostream &out = writer.stream();
	CPPUNIT_ASSERT(testingTmpFile().exists());
	CPPUNIT_ASSERT(out.good());

	out << "avoid target file";
	CPPUNIT_ASSERT(out.good());
	writer.flush();
	CPPUNIT_ASSERT(out.good());

	CPPUNIT_ASSERT_FILE_TEXTUAL_EQUALS("some data", testingFile());
	CPPUNIT_ASSERT_FILE_TEXTUAL_EQUALS("avoid target file", testingTmpFile());

	out << " completed";
	CPPUNIT_ASSERT(out.good());
	writer.reset();

	CPPUNIT_ASSERT(!testingTmpFile().exists());
	CPPUNIT_ASSERT_FILE_TEXTUAL_EQUALS("some data", testingFile());
}

/**
 * @brief Test we can force re-creating the tmp file if it already exists.
 * We create the tmp-file as a symlink to /dev/null. In this way, we can
 * check whether the SafeWriter removes and re-creates such tmp-file
 * (the link is replaced by a regular file).
 */
void SafeWriterTest::testTmpExists()
{
	CPPUNIT_ASSERT_NO_THROW(createLink("/dev/null", testingTmpFile()));
	CPPUNIT_ASSERT(testingTmpFile().exists());
	CPPUNIT_ASSERT(testingTmpFile().isLink());

	SafeWriter writer(testingTmpFile());

	CPPUNIT_ASSERT_THROW(writer.stream(), FileExistsException);
	CPPUNIT_ASSERT(testingTmpFile().isLink());

	CPPUNIT_ASSERT_NO_THROW(writer.stream(true));
	CPPUNIT_ASSERT(!testingTmpFile().isLink());
	CPPUNIT_ASSERT(testingTmpFile().isFile());
}

/**
 * @brief Test behaviour of two conflicting writers. NO, this is NOT a proper test
 * of race conditions! The second writer can write after the first one commits
 * (when force is not true).
 */
void SafeWriterTest::testConflictingWriters()
{
	SafeWriter writer0(testingTmpFile());
	SafeWriter writer1(testingTmpFile());

	ostream &out0 = writer0.stream();
	CPPUNIT_ASSERT_THROW(writer1.stream(), FileExistsException);

	out0 << "some content";
	writer0.commitAs(testingPath());
	CPPUNIT_ASSERT_FILE_TEXTUAL_EQUALS("some content", testingFile());

	ostream &out1 = writer1.stream();
	out1 << "some other content";
	writer1.commitAs(testingPath());
	CPPUNIT_ASSERT_FILE_TEXTUAL_EQUALS("some other content", testingFile());
}

/**
 * @brief Test we are optionally able to check proper write with
 * application SHA-1 checksum.
 */
void SafeWriterTest::testFinalize()
{
	CPPUNIT_ASSERT_FILE_TEXTUAL_EQUALS("", testingFile());

	SafeWriter writer(testingTmpFile());

	ostream &out = writer.stream();
	out << "some data";
	writer.flush();

	CPPUNIT_ASSERT_FILE_TEXTUAL_EQUALS("some data", testingTmpFile());

	SHA1Engine engine;
	engine.update("some data", 9);

	const auto state = writer.finalize();

	CPPUNIT_ASSERT_FILE_TEXTUAL_EQUALS("", testingFile());
	CPPUNIT_ASSERT_FILE_TEXTUAL_EQUALS("some data", testingTmpFile());

	const auto &digest = engine.digest();
	CPPUNIT_ASSERT_EQUAL(string("some data").size(), state.second);
	CPPUNIT_ASSERT(state.first == digest);

	CPPUNIT_ASSERT_NO_THROW(writer.commitAs(testingPath()));
}

/**
 * @brief Test behaviour in case of writing a too big file. The file size limit
 * is ensured via the setrlimit call. We can simulate a kind of storage media
 * error in this way.
 */
void SafeWriterTest::testTooBig()
{
	CPPUNIT_ASSERT_NO_THROW(testingFile().remove());

	SafeWriter writer(testingTmpFile());

	CPPUNIT_ASSERT_NO_THROW(updateFileRlimit(128));
	PosixSignal::ignore("SIGXFSZ");

	ostream &out = writer.stream();
	// write 150 bytes of data
	out << "0123456789" << "0123456789" << "0123456789" << "0123456789" << "0123456789";
	out << "0123456789" << "0123456789" << "0123456789" << "0123456789" << "0123456789";
	writer.flush();

	CPPUNIT_ASSERT(out.good());

	out << "0123456789" << "0123456789" << "0123456789" << "0123456789" << "0123456789";
	writer.flush();

	CPPUNIT_ASSERT(out.bad());

	CPPUNIT_ASSERT_THROW(writer.commitAs(testingPath()), WriteFileException);
	CPPUNIT_ASSERT_FILE_EXISTS(testingTmpFile());
	CPPUNIT_ASSERT_FILE_NOT_EXISTS(testingPath());

	CPPUNIT_ASSERT_NO_THROW(writer.reset());
	CPPUNIT_ASSERT_FILE_NOT_EXISTS(testingTmpFile());
	CPPUNIT_ASSERT_FILE_NOT_EXISTS(testingPath());
}

}
