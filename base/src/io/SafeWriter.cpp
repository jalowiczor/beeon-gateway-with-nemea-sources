#include <Poco/Exception.h>
#include <Poco/File.h>
#include <Poco/Logger.h>
#include <Poco/NullStream.h>
#include <Poco/StreamCopier.h>

#include "io/SafeWriter.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

static File appendExtension(const File &file, const string &ext)
{
	Path path(file.path());
	const auto &origExt = path.getExtension();

	if (origExt.empty())
		path.setExtension(ext);
	else
		path.setExtension(origExt + "." + ext);

	return path;
}

SafeWriter::SafeWriter(const File &file, const string &ext):
	SafeWriter(appendExtension(file, ext))
{
}

SafeWriter::SafeWriter(const File &file):
	m_tmpFile(file),
	m_digestStream(m_engine, m_fileStream),
	m_countingStream(m_digestStream),
	m_recentLength(0),
	m_stream(m_countingStream),
	m_created(false),
	m_finalized(false),
	m_committed(false)
{
}

SafeWriter::~SafeWriter()
{
	if (m_committed)
		return;

	try {
		m_tmpFile.remove();
	}
	catch (const FileNotFoundException &e) {
		if (!m_created)
			return;

		logger().log(e, __FILE__, __LINE__);
	}
	BEEEON_CATCH_CHAIN(logger())
}

ostream &SafeWriter::stream(bool force)
{
	if (m_committed) {
		throw IllegalStateException(
			"file " + m_tmpFile.path()
			+ " already committed");
	}

	if (!m_created) {
		if (logger().debug()) {
			logger().debug("open and trunc " + m_tmpFile.path(),
				__FILE__, __LINE__);
		}

		createLockFile(force);

		m_fileStream.open(m_tmpFile.path(),
			ios::out | ios::trunc | ios::binary);

		m_created = true;
	}

	return m_stream;
}

void SafeWriter::createLockFile(bool force)
{
	while (!m_tmpFile.createFile()) {
		if (!force) {
			throw FileExistsException(
				"file " + m_tmpFile.path() + " already exists");
		}

		try {
			m_tmpFile.remove();
		}
		catch (const FileNotFoundException &e) {
			logger().warning(e.message(), __FILE__, __LINE__);
		}
	}
}

void SafeWriter::flush()
{
	m_digestStream.flush();
	m_fileStream.flush();
	m_stream.setstate(m_fileStream.rdstate());
}

void SafeWriter::reset()
{
	if (m_committed)
		throw IllegalStateException("cannot reset committed state");

	flush();

	m_stream.clear();
	m_digestStream.clear();
	m_fileStream.close();
	m_countingStream.reset();

	m_engine.reset();
	m_recentDigest.clear();
	m_recentLength = 0;

	try {
		m_tmpFile.remove();
	}
	catch (const FileNotFoundException &) {
		// Lock file does not exist yet.
		// We are interested in serious IO exceptions, not this one.
	}

	m_finalized = false;
	m_created = false;
}

pair<DigestEngine::Digest, size_t> SafeWriter::finalize()
{
	if (!m_finalized) {
		flush();

		if (!m_stream.good())
			throw WriteFileException("failed to flush into " + m_tmpFile.path());

		m_recentDigest = m_engine.digest();
		m_recentLength = m_countingStream.chars();
		m_finalized = true;

		if (logger().debug()) {
			logger().debug(
				"finalized after written " + to_string(m_recentLength) + " B into "
				+ m_tmpFile.path() + " with digest "
				+ DigestEngine::digestToHex(m_recentDigest),
				__FILE__, __LINE__);
		}
	}

	return make_pair(m_recentDigest, m_recentLength);
}

void SafeWriter::commitAs(const File &targetFile)
{
	if (m_committed) {
		throw IllegalStateException(
			"file " + m_tmpFile.path() + " was already committed");
	}

	const auto &state = finalize();

	SHA1Engine check;
	FileInputStream fin(m_tmpFile.path());
	DigestInputStream din(check, fin);
	CountingInputStream in(din);
	NullOutputStream null;

	StreamCopier::copyStream(in, null);

	const auto &afterRead = check.digest();
	const size_t length = in.chars();

	if (m_recentLength != length) {
		throw WriteFileException(
			"re-read " + to_string(length) + " B but expected "
			+ to_string(state.second) + " B");
	}

	if (m_recentDigest != afterRead) {
		throw WriteFileException(
			"re-read digest check failed "
			+ DigestEngine::digestToHex(state.first)
			+ " != "
			+ DigestEngine::digestToHex(afterRead));
	}

	m_tmpFile.renameTo(targetFile.path());

	if (logger().debug()) {
		logger().debug(
			"file " + targetFile.path() + " committed",
			__FILE__, __LINE__);
	}

	m_committed = true;
}
