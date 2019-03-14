#include <Poco/Exception.h>
#include <Poco/Logger.h>

#include "loop/StopControl.h"
#include "util/Loggable.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

StopControl::Run::Run(StopControl &control):
	m_control(control)
{
	m_control.clear();
}

StopControl::Run::~Run()
{
	try {
		m_control.clear();
	}
	BEEEON_CATCH_CHAIN(Loggable::forInstance(this))
}

bool StopControl::Run::waitStoppable(const Timespan &timeout)
{
	return m_control.waitStoppable(timeout);
}

StopControl::Run::operator bool() const
{
	return !m_control.shouldStop();
}

StopControl::StopControl():
	m_stop(false)
{
}

bool StopControl::doWait(long ms)
{
	if (ms < 0) {
		m_condition.wait(m_lock);
		return true;
	}

	return m_condition.tryWait(m_lock, ms);
}

bool StopControl::waitStoppable(const Timespan &timeout)
{
	FastMutex::ScopedLock guard(m_lock);

	if (m_stop)
		return true; // we are always woken up here

	if (timeout < 0)
		return doWait(-1);

	if (timeout < 1 * Timespan::MILLISECONDS)
		return doWait(1);

	return doWait(timeout.totalMilliseconds());
}

bool StopControl::shouldStop() const
{
	FastMutex::ScopedLock guard(m_lock);

	return m_stop;
}

void StopControl::requestStop()
{
	FastMutex::ScopedLock guard(m_lock);

	m_stop = true;
	requestWakeup();
}

void StopControl::requestWakeup()
{
	m_condition.broadcast();
}

void StopControl::clear()
{
	FastMutex::ScopedLock guard(m_lock);

	m_stop = false;
}
