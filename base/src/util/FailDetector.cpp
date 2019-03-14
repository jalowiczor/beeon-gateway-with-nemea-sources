#include "util/FailDetector.h"

using namespace BeeeOn;
using namespace Poco;

FailDetector::FailDetector(const unsigned int failsTreshold):
	m_fails(0)
{
	m_failsTreshold = failsTreshold;
}

FailDetector::~FailDetector()
{
}

void FailDetector::fail()
{
	m_timeOfLastFail.update();

	if (!isFailed()) {
		if (++m_fails && isFailed())
			m_timeOfFailure.update();
	}
}

void FailDetector::success()
{
	m_fails = 0;
}

bool FailDetector::isGoingToFail() const
{
	return m_fails > 0;
}

bool FailDetector::isFailed() const
{
	return (m_fails >= m_failsTreshold) && (m_failsTreshold > 0);
}

Timestamp FailDetector::timeOfLastFail() const
{
	return m_timeOfLastFail;
}

bool FailDetector::lastFailBefore(const Timespan &timeout) const
{
	return m_timeOfLastFail.isElapsed(timeout.totalMicroseconds());
}

Timestamp FailDetector::timeOfFailure() const
{
	return m_timeOfFailure;
}

bool FailDetector::failedBefore(const Timespan &timeout) const
{
	return m_timeOfFailure.isElapsed(timeout.totalMicroseconds());
}
