#include <Poco/Exception.h>

#include "util/WaitCondition.h"

using namespace Poco;
using namespace BeeeOn;

WaitCondition::Broadcaster::Broadcaster(WaitCondition &condition):
	m_broadcasted(false),
	m_condition(condition)
{
}

WaitCondition::Broadcaster::~Broadcaster()
{
	broadcast();
}

void WaitCondition::Broadcaster::broadcast()
{
	if (!m_broadcasted)
		m_condition.broadcast();

	m_broadcasted = true;
}

WaitCondition::WaitCondition(bool repeat):
	m_canRepeat(repeat),
	m_broadcasted(false)
{
}

WaitCondition::~WaitCondition()
{
}

long WaitCondition::toMilliseconds(const Timespan &timeout) const
{
	if (timeout < 0)
		return -1;

	if (timeout < 1 * Timespan::MILLISECONDS)
		return 1;

	return timeout.totalMilliseconds();
}

void WaitCondition::wait(const Timespan &timeout)
{
	if (!doWait(toMilliseconds(timeout)))
		throw TimeoutException("condition was not met while waiting");
}

bool WaitCondition::tryWait(const Timespan &timeout)
{
	return doWait(toMilliseconds(timeout));
}

bool WaitCondition::doWait(long ms)
{
	FastMutex::ScopedLock guard(m_lock);

	if (m_broadcasted)
		return true;

	if (ms < 0) {
		m_condition.wait(m_lock);
		return true;
	}
	else {
		return m_condition.tryWait(m_lock, ms);
	}
}

void WaitCondition::broadcast()
{
	m_broadcasted = !m_canRepeat;
	m_condition.broadcast();
}
