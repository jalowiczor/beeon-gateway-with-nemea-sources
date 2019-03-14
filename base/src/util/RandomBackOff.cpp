#include <Poco/Exception.h>

#include "di/Injectable.h"
#include "util/RandomBackOff.h"

BEEEON_OBJECT_BEGIN(BeeeOn, RandomBackOffFactory)
BEEEON_OBJECT_CASTABLE(BackOffFactory)
BEEEON_OBJECT_PROPERTY("min", &RandomBackOffFactory::setMin)
BEEEON_OBJECT_PROPERTY("max", &RandomBackOffFactory::setMax)
BEEEON_OBJECT_PROPERTY("count", &RandomBackOffFactory::setCount)
BEEEON_OBJECT_PROPERTY("seed", &RandomBackOffFactory::setSeed)
BEEEON_OBJECT_HOOK("done", &RandomBackOffFactory::validate)
BEEEON_OBJECT_END(BeeeOn, RandomBackOffFactory)

using namespace Poco;
using namespace BeeeOn;

const uint32_t RandomBackOff::RANDOM_SEED = 0xffffffff;

RandomBackOff::RandomBackOff(
		const Timespan &min,
		const Timespan &max,
		size_t count,
		uint32_t seed):
	m_min(min),
	m_max(max),
	m_count(count),
	m_current(0),
	m_seed(seed)
{
	poco_assert(min < max);

	if (m_seed == RANDOM_SEED)
		m_random.seed();
	else
		m_random.seed(m_seed);
}

Timespan RandomBackOff::next()
{
	if (m_count > 0 && m_current++ >= m_count)
		return STOP;

	const auto diff = m_max - m_min;
	poco_assert(diff <= 0x0ffffffffL);

	const auto us = m_random.next(diff.totalMicroseconds());
	return m_min + us;
}

void RandomBackOff::reset()
{
	m_current = 0;

	if (m_seed == RANDOM_SEED)
		m_random.seed();
	else
		m_random.seed(m_seed);
}

RandomBackOffFactory::RandomBackOffFactory():
	m_min(0),
	m_max(0),
	m_count(0),
	m_seed(RandomBackOff::RANDOM_SEED)
{
}

void RandomBackOffFactory::setMin(const Timespan &min)
{
	m_min = min;
}

void RandomBackOffFactory::setMax(const Timespan &max)
{
	m_max = max;
}

void RandomBackOffFactory::setCount(int count)
{
	if (count < 0)
		throw InvalidArgumentException("count must not be negative");

	m_count = count;
}

void RandomBackOffFactory::setSeed(int seed)
{
	m_seed = static_cast<uint32_t>(seed);
}

void RandomBackOffFactory::validate()
{
	if (m_min >= m_max)
		throw InvalidArgumentException("min must be smaller then max");
}

BackOff::Ptr RandomBackOffFactory::create()
{
	return new RandomBackOff(m_min, m_max, m_count, m_seed);
}
