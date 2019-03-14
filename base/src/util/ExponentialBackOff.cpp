#include <Poco/Exception.h>

#include "di/Injectable.h"
#include "util/ExponentialBackOff.h"

BEEEON_OBJECT_BEGIN(BeeeOn, ExponentialBackOffFactory)
BEEEON_OBJECT_CASTABLE(BackOffFactory)
BEEEON_OBJECT_PROPERTY("initialInterval", &ExponentialBackOffFactory::setInitialInterval)
BEEEON_OBJECT_PROPERTY("maxInterval", &ExponentialBackOffFactory::setMaxInterval)
BEEEON_OBJECT_PROPERTY("maxElapsedTime", &ExponentialBackOffFactory::setMaxElapsedTime)
BEEEON_OBJECT_PROPERTY("randomizationFactor",
		&ExponentialBackOffFactory::setRandomizationFactor)
BEEEON_OBJECT_PROPERTY("multiplier", &ExponentialBackOffFactory::setMultiplier)
BEEEON_OBJECT_END(BeeeOn, ExponentialBackOffFactory)

using namespace Poco;
using namespace BeeeOn;

ExponentialBackOff::Config::Config():
	m_initialInterval(500 * Timespan::MILLISECONDS),
	m_maxInterval(1 * Timespan::MINUTES),
	m_maxElapsedTime(15 * Timespan::MINUTES),
	m_randomizationFactor(0.5),
	m_multiplier(1.5)
{
}

void ExponentialBackOff::Config::setInitialInterval(const Timespan &interval)
{
	if (interval <= 0)
		throw InvalidArgumentException("initial interval must be positive");

	m_initialInterval = interval;
}

Timespan ExponentialBackOff::Config::initialInterval() const
{
	return m_initialInterval;
}

void ExponentialBackOff::Config::setMaxInterval(const Timespan &interval)
{
	if (interval < 0)
		throw InvalidArgumentException("max interval must not be negative");

	m_maxInterval = interval;
}

Timespan ExponentialBackOff::Config::maxInterval() const
{
	return m_maxInterval;
}

void ExponentialBackOff::Config::setMaxElapsedTime(const Timespan &elapsed)
{
	m_maxElapsedTime = elapsed;
}

Timespan ExponentialBackOff::Config::maxElapsedTime() const
{
	return m_maxElapsedTime;
}

void ExponentialBackOff::Config::setRandomizationFactor(double f)
{
	if (f < 0 || f > 1)
		throw InvalidArgumentException("randomization factor must be in range 0..1");

	m_randomizationFactor = f;
}

double ExponentialBackOff::Config::randomizationFactor() const
{
	return m_randomizationFactor;
}

void ExponentialBackOff::Config::setMultiplier(double m)
{
	if (m < 0)
		throw InvalidArgumentException("multiplier must not be negative");

	m_multiplier = m;
}

double ExponentialBackOff::Config::multiplier() const
{
	return m_multiplier;
}


ExponentialBackOff::ExponentialBackOff():
	m_nextInterval(m_config.initialInterval())
{
	m_random.seed();
}

void ExponentialBackOff::applyConfig(const Config &config)
{
	m_config = config;
}

const ExponentialBackOff::Config &ExponentialBackOff::config() const
{
	return m_config;
}

Timespan ExponentialBackOff::initialInterval() const
{
	return m_config.initialInterval();
}

Timespan ExponentialBackOff::maxInterval() const
{
	return m_config.maxInterval();
}

Timespan ExponentialBackOff::maxElapsedTime() const
{
	return m_config.maxElapsedTime();
}

double ExponentialBackOff::randomizationFactor() const
{
	return m_config.randomizationFactor();
}

double ExponentialBackOff::multiplier() const
{
	return m_config.multiplier();
}

Timespan ExponentialBackOff::nextInterval() const
{
	return m_nextInterval;
}

Timespan ExponentialBackOff::elapsed() const
{
	return m_resetStamp.elapsed();
}

Timespan ExponentialBackOff::next()
{
	if (m_resetStamp.isElapsed(maxElapsedTime().totalMicroseconds()))
		return STOP;

	const Timespan value = {nextRandom()};
	m_nextInterval = m_nextInterval.totalMilliseconds()
		* multiplier()
		* Timespan::MILLISECONDS;

	return value;
}

Timespan ExponentialBackOff::nextRandom()
{
	const Timespan::TimeDiff from =
		m_nextInterval.totalMicroseconds() * (1 - randomizationFactor());
	const Timespan::TimeDiff to =
		m_nextInterval.totalMicroseconds() * (1 + randomizationFactor());

	const Timespan::TimeDiff diff = to - from;
	if (diff < 0)
		throw IllegalStateException("back-off range is invalid");

	const double r = m_random.nextDouble();
	return from + (r * diff);
}

void ExponentialBackOff::reset()
{
	m_random.seed();
	m_nextInterval = initialInterval();
	m_resetStamp.update();
}

BackOff::Ptr ExponentialBackOffFactory::create()
{
	ExponentialBackOff::Ptr instance = new ExponentialBackOff;
	instance->applyConfig(m_config);

	return instance;
}

void ExponentialBackOffFactory::setInitialInterval(const Timespan &interval)
{
	m_config.setInitialInterval(interval);
}

void ExponentialBackOffFactory::setMaxInterval(const Timespan &interval)
{
	m_config.setMaxInterval(interval);
}

void ExponentialBackOffFactory::setMaxElapsedTime(const Timespan &time)
{
	m_config.setMaxElapsedTime(time);
}

void ExponentialBackOffFactory::setRandomizationFactor(double factor)
{
	m_config.setRandomizationFactor(factor);
}

void ExponentialBackOffFactory::setMultiplier(double multiplier)
{
	m_config.setMultiplier(multiplier);
}
