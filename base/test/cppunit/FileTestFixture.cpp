#include <unistd.h>

#include <sstream>

#include <Poco/DirectoryIterator.h>
#include <Poco/Error.h>
#include <Poco/Exception.h>
#include <Poco/FileStream.h>
#include <Poco/StreamCopier.h>
#include <Poco/HexBinaryEncoder.h>

#include "cppunit/FileTestFixture.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

void FileTestFixture::setUp()
{
	setUpAsFile();
}

void FileTestFixture::setUpAsFile()
{
	obtainOriginalFileRlimit();

	try {
		m_file.setWriteable(true);
		m_file.remove();
	}
	catch (const FileNotFoundException &) {
		// this is possible and OK, no panic
	}

	File(testingPath().parent()).createDirectories();
	CPPUNIT_ASSERT(m_file.createFile());
}

void FileTestFixture::setUpAsDirectory()
{
	obtainOriginalFileRlimit();

	try {
		m_file.setWriteable(true);
		m_file.remove(true);
	}
	catch (const FileNotFoundException &) {
		// this is possible and OK, no panic
	}

	File(testingPath().parent()).createDirectories();
	CPPUNIT_ASSERT(m_file.createDirectory());
}

void FileTestFixture::tearDown()
{
	try {
		if (m_file.isLink()) {
			m_file.remove();
		}
		else {
			m_file.setWriteable(true);
			m_file.remove(true);
		}
	}
	catch (const FileNotFoundException &) {
		// this is possible and OK, no panic
	}

	rollbackOriginalFileRlimit();
}

void FileTestFixture::obtainOriginalFileRlimit()
{
	if (getrlimit(RLIMIT_FSIZE, &m_originalFileRlimit) < 0)
		throw SystemException("getrlimit: " + Error::getMessage(Error::last()));
}

void FileTestFixture::rollbackOriginalFileRlimit()
{
	if (setrlimit(RLIMIT_FSIZE, &m_originalFileRlimit) < 0)
		throw SystemException("setrlimit: " + Error::getMessage(Error::last()));
}

struct rlimit FileTestFixture::originalFileRlimit() const
{
	return m_originalFileRlimit;
}

void FileTestFixture::updateFileRlimit(size_t lim) const
{
	struct rlimit copy = m_originalFileRlimit;
	copy.rlim_cur = lim;
	CPPUNIT_ASSERT(copy.rlim_cur <= copy.rlim_max
			|| copy.rlim_max == RLIM_INFINITY);

	if (setrlimit(RLIMIT_FSIZE, &copy) < 0)
		throw SystemException("setrlimit: " + Error::getMessage(Error::last()));
}

File &FileTestFixture::testingFile()
{
	return m_file;
}

Path FileTestFixture::testingPath() const
{
	return Path(m_file.path());
}

string FileTestFixture::readFile(const File &file) const
{
	FileInputStream fin(file.path());
	string buffer;
	StreamCopier::copyToString(fin, buffer);

	return buffer;
}

void FileTestFixture::writeFile(const File &file, const string &content) const
{
	FileOutputStream fout(file.path());
	fout << content;
	fout.flush();
}

void FileTestFixture::createLink(
	const Path &existing,
	const File &target,
	bool hardlink) const
{
	int ret;

	const auto &old = existing.toString();
	const auto &name = target.path();

	if (hardlink) {
		ret = ::link(old.c_str(), name.c_str());
	}
	else {
		ret = ::symlink(old.c_str(), name.c_str());
	}

	if (ret < 0)
		throw CreateFileException(Error::getMessage(Error::last()));
}

void FileTestFixture::assertFileExists(
		const File &file,
		const CppUnit::SourceLine sourceLine) const
{
	CppUnit::Asserter::failIf(
		!file.exists(),
		"file " + file.path() + " does not exist",
		sourceLine);
}

void FileTestFixture::assertFileNotExists(
		const File &file,
		const CppUnit::SourceLine sourceLine) const
{
	CppUnit::Asserter::failIf(
		file.exists(),
		"file " + file.path() + " does exist but should not",
		sourceLine);
}

void FileTestFixture::assertDirectoryEmpty(
	const File &file,
	const CppUnit::SourceLine sourceLine) const
{
	CppUnit::Asserter::failIf(
		!file.isDirectory(),
		"file " + file.path() + " is not a directory",
		sourceLine);

	DirectoryIterator it(file);
	const DirectoryIterator end;

	for (; it != end; ++it) {
		CppUnit::Asserter::fail(
			"directory " + file.path() + " is not empty: " + it.name(),
			sourceLine);
	}
}

void FileTestFixture::assertFileTextualEquals(
		const string &content,
		const File &file,
		const CppUnit::SourceLine sourceLine) const
{
	assertFileExists(file, sourceLine);
	CppUnit::assertEquals(
		content,
		readFile(file),
		sourceLine,
		"file " + file.path() + " has unexpected textual content");
}

void FileTestFixture::assertFileBinaryEquals(
		const string &content,
		const File &file,
		const CppUnit::SourceLine sourceLine) const
{
	assertFileExists(file, sourceLine);

	ostringstream fbuffer;
	HexBinaryEncoder fencoder(fbuffer);
	FileInputStream fin(file.path());
	StreamCopier::copyStream(fin, fencoder);
	fencoder.flush();

	ostringstream cbuffer;
	HexBinaryEncoder cencoder(cbuffer);
	cencoder << content;
	cencoder.flush();

	assertEquals(
		cbuffer.str(),
		fbuffer.str(),
		sourceLine,
		"file " + file.path() + " has unexpected binary content");
}
