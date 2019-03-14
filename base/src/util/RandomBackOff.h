#pragma once

#include <Poco/Random.h>
#include <Poco/Timespan.h>

#include "util/BackOff.h"

namespace BeeeOn {

/**
 * @brief Back-off policy that generates random delays in range
 * between the given min and max. The random generation can be
 * explicitly seeded for deterministic results (otherwise, it
 * is seeded from the system's entropy pool). The RandomBackOff
 * generates the specified number of random delays or inifinite
 * based on the property count.
 */
class RandomBackOff : public BackOff {
public:
	static const uint32_t RANDOM_SEED;

	RandomBackOff(
		const Poco::Timespan &min,
		const Poco::Timespan &max,
		size_t count,
		uint32_t seed);

	/**
	 * @brief Generates up to count of random delays.
	 * If the count is set to 0, it is treated as inifinity.
	 *
	 * @returns random delay between min and max or STOP
	 * when the configure count of generations was reached
	 */
	Poco::Timespan next() override;

	/**
	 * @brief Re-seed and clear the number of available
	 * iterations to zero.
	 */
	void reset() override;

private:
	const Poco::Timespan m_min;
	const Poco::Timespan m_max;
	const size_t m_count;
	size_t m_current;
	uint32_t m_seed;
	Poco::Random m_random;
};

/**
 * @brief RandomBackOffFactory creates a preconfigured instances
 * of class RandomBackOff.
 */
class RandomBackOffFactory : public BackOffFactory {
public:
	RandomBackOffFactory();

	void setMin(const Poco::Timespan &min);
	void setMax(const Poco::Timespan &max);
	void setCount(int count);
	void setSeed(int seed);

	void validate();

	BackOff::Ptr create() override;

private:
	Poco::Timespan m_min;
	Poco::Timespan m_max;
	size_t m_count;
	uint32_t m_seed;
};

}
