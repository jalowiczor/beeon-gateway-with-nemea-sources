#include "io/IOStats.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

string IOStats::Data::toString() const
{
	return to_string(writtenPdu)
		+ "/"
		+ to_string(writtenBytes)
		+ " B/"
		+ to_string(readPdu)
		+ "/"
		+ to_string(readBytes)
		+ " B/"
		+ to_string(lostPdu)
		+ "/"
		+ to_string(lostBytes)
		+ " B";
}

IOStats::IOStats():
	m_data({0, 0, 0, 0, 0, 0})
{
}

void IOStats::written(size_t bytes)
{
	RWLock::ScopedWriteLock guard(m_lock);

	m_data.writtenPdu += 1;
	m_data.writtenBytes += bytes;
}

void IOStats::read(size_t bytes)
{
	RWLock::ScopedWriteLock guard(m_lock);

	m_data.readPdu += 1;
	m_data.readBytes += bytes;
}

void IOStats::lost(size_t bytes)
{
	RWLock::ScopedWriteLock guard(m_lock);

	m_data.lostPdu += 1;
	m_data.lostBytes += bytes;
}

IOStats::Data IOStats::data() const
{
	RWLock::ScopedReadLock guard(m_lock);

	return m_data;
}
