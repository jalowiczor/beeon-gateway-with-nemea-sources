#pragma once

#include <string>
#include <utility>

#include <Poco/CountingStream.h>
#include <Poco/DigestStream.h>
#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/Path.h>
#include <Poco/SHA1Engine.h>
#include <Poco/TemporaryFile.h>

#include "util/Loggable.h"

namespace BeeeOn {

/**
 * @brief SafeWriter is a helper that allows to perform file writes
 * that are atomic. SafeWriter always rewrites a whole file. However,
 * it either rewrites that file or it does nothing.
 *
 * The actual writes are performed into a temporary file first. If
 * writing fails. the file is simply removed. If writing succeeds,
 * the SafeWriter checks it for consistency (via SHA-1 checksum)
 * and if the stored file's checksum is correct, it renames the
 * temporary file to the target file (this should be atomic
 * operation on most operating systems).
 */
class SafeWriter : Loggable {
public:
	SafeWriter(const Poco::File &tmpFile);
	SafeWriter(const Poco::File &tmpFile, const std::string &ext);
	~SafeWriter();

	/**
	 * @returns the underlying output stream unless already committed
	 * @throws Poco::IllegalStateException in the case when the writter
	 * has already been committed.
	 */
	std::ostream &stream(bool force = false);

	/**
	 * @brief Flush the given stream. It is recommended to prefer this
	 * call instead of stream().flush() because the underlying stream
	 * implementation might not propagate flushing properly.
	 */
	void flush();

	/**
	 * @brief Reset the writer and allow to try writing again before commit
	 * succeeds.
	 * @throws IllegalStateException if already committed
	 */
	void reset();

	/**
	 * Finalize writing into the underlying stream. This is not a commit
	 * but no more writes are possible after this call.
	 *
	 * @returns digest and size of the written data
	 */
	std::pair<Poco::DigestEngine::Digest, size_t> finalize();

	/**
	 * @brief Commit the prepared temporary file as follows:
	 * - flush the underlying output stream
	 * - read back the temporary file and validate checksum of read-back data
	 * - rename the temporary lock file to the target destination
	 */
	void commitAs(const Poco::File &targetFile);

protected:
	/**
	 * @brief Create the temporary lock file atomically. It fails in the case when
	 * the lock file already exists unless force is true. If the parameter force is
	 * true, then the already existing temporary lock file is first removed and
	 * then again recreated. We continue this process until we successfully create
	 * the temporary file atomically.
	 *
	 * @throws FileExistsException when the lock file already exists
	 */
	void createLockFile(bool force);

private:
	Poco::File m_tmpFile;
	Poco::FileOutputStream m_fileStream;
	Poco::SHA1Engine m_engine;
	Poco::DigestOutputStream m_digestStream;
	Poco::DigestEngine::Digest m_recentDigest;
	Poco::CountingOutputStream m_countingStream;
	size_t m_recentLength;
	std::ostream &m_stream;
	bool m_created;
	bool m_finalized;
	bool m_committed;
};

}
