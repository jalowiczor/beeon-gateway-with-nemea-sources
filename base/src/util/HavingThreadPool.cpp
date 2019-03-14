#include <Poco/Logger.h>

#include "util/HavingThreadPool.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

HavingThreadPool::HavingThreadPool():
	m_minThreads(1),
	m_maxThreads(16),
	m_threadIdleTime(5 * Timespan::SECONDS)
{
}

HavingThreadPool::~HavingThreadPool()
{
}

void HavingThreadPool::setMinThreads(int min)
{
	if (min < 0)
		throw InvalidArgumentException("minThreads must be non-negative");

	m_minThreads = min;
}

void HavingThreadPool::setMaxThreads(int max)
{
	if (max < 0)
		throw InvalidArgumentException("maxThreads must be non-negative");

	m_maxThreads = max;
}

void HavingThreadPool::setThreadIdleTime(const Timespan &time)
{
	if (time.totalSeconds() <= 0)
		throw InvalidArgumentException("threadIdleTime must be at least 1 second");

	m_threadIdleTime = time;
}

void HavingThreadPool::initPool()
{
	if (m_pool.isNull()) {
		logger().notice("creating thread pool min: "
			+ to_string(m_minThreads)
			+ " max: "
			+ to_string(m_maxThreads),
			__FILE__, __LINE__);

		m_pool = new ThreadPool(
			m_minThreads,
			m_maxThreads,
			m_threadIdleTime.totalSeconds()
		);
	}
}

ThreadPool &HavingThreadPool::pool()
{
	initPool();
	return *m_pool;
}
