#pragma once

#include <sys/resource.h>
#include <string>

#include <cppunit/extensions/HelperMacros.h>

#include <Poco/FileStream.h>
#include <Poco/Path.h>
#include <Poco/TemporaryFile.h>

namespace BeeeOn {

/**
 * @brief FileTestFixture is an abstract class useful to write
 * filesystem-related unit tests. Such unit tests usually need
 * a temporary file or directory to work with. The FileTestFixture
 * creates such temporary location and cleans it up on tear down.
 */
class FileTestFixture : public CppUnit::TestFixture {
public:
	/**
	 * @brief Initializes the following test case by calling to setUpAsFile().
	 * It is expected that setUpAsFile() is the most commonly wanted setup.
	 * Others must override and call setUpAsDirectory().
	 */
	void setUp() override;

	/**
	 * @brief Deletes the testing file (recursively if needed).
	 */
	void tearDown() override;

protected:
	/**
	 * @brief Create the testing temporary file as a regular file.
	 */
	void setUpAsFile();

	/**
	 * @brief Create the testing temporary file as a directory.
	 */
	void setUpAsDirectory();

	void obtainOriginalFileRlimit();
	void rollbackOriginalFileRlimit();

	struct rlimit originalFileRlimit() const;
	void updateFileRlimit(size_t size) const;

	Poco::File &testingFile();
	Poco::Path testingPath() const;

	std::string readFile(const Poco::File &file) const;
	void writeFile(const Poco::File &file, const std::string &content) const;

	void createLink(
		const Poco::Path &existing,
		const Poco::File &target,
		bool hardlink = false) const;

	void assertFileExists(
		const Poco::File &file,
		const CppUnit::SourceLine sourceLine) const;

	void assertFileNotExists(
		const Poco::File &file,
		const CppUnit::SourceLine sourceLine) const;

	void assertDirectoryEmpty(
		const Poco::File &file,
		const CppUnit::SourceLine sourceLine) const;

	void assertFileTextualEquals(
		const std::string &actual,
		const Poco::File &file,
		const CppUnit::SourceLine sourceLine) const;

	void assertFileBinaryEquals(
		const std::string &actual,
		const Poco::File &file,
		const CppUnit::SourceLine sourceLine) const;

private:
	Poco::TemporaryFile m_file;
	struct rlimit m_originalFileRlimit;
};

#define CPPUNIT_ASSERT_FILE_EXISTS(file) \
	assertFileExists(file, CPPUNIT_SOURCELINE())
#define CPPUNIT_ASSERT_FILE_NOT_EXISTS(file) \
	assertFileNotExists(file, CPPUNIT_SOURCELINE())
#define CPPUNIT_ASSERT_FILE_TEXTUAL_EQUALS(content, file) \
	assertFileTextualEquals(content, file, CPPUNIT_SOURCELINE())
#define CPPUNIT_ASSERT_FILE_BINARY_EQUALS(content, file) \
	assertFileBinaryEquals(content, file, CPPUNIT_SOURCELINE())
#define CPPUNIT_ASSERT_DIR_EMPTY(file) \
	assertDirectoryEmpty(file, CPPUNIT_SOURCELINE())

}
