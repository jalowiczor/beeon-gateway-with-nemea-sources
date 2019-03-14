#include <Poco/Exception.h>
#include <Poco/Logger.h>

#include "util/PeriodicRunner.h"

using namespace Poco;
using namespace BeeeOn;

PeriodicRunner::PeriodicRunner():
	m_invoke(*this, &PeriodicRunner::onStart)
{
}

PeriodicRunner::~PeriodicRunner()
{
}

void PeriodicRunner::start(const Callback &callback)
{
	Timer::stop();

	m_callback = callback;
	Timer::setPeriodicInterval(m_interval.totalMilliseconds());
	Timer::start(m_invoke);
}

void PeriodicRunner::stop()
{
	Timer::stop();
}

void PeriodicRunner::setInterval(const Timespan &interval)
{
	if (interval.totalMilliseconds() <= 0)
		throw InvalidArgumentException("too small interval for period invocations");

	m_interval = interval;
}

void PeriodicRunner::onStart(Timer &)
{
	try {
		m_callback();
	}
	BEEEON_CATCH_CHAIN(logger())
}
