#pragma once

#include <Poco/Clock.h>
#include <Poco/Random.h>

#include "util/BackOff.h"

namespace BeeeOn {

/**
 * @brief ExponentialBackOff implements the computation of delay when retrying
 * an operation. It is usually used to lower load of a server.
 *
 * The instance manages a time interval (e.g. 5 seconds) suitable for random
 * generation of the delay (example random delayes: 1 second, 3,5 seconds).
 * The time interval changes in each iteration by applying the multiplier
 * (configures the expenential grow speed).
 *
 * The time interval is in fact represented as <min, max>. The min and max are
 * computed from the current time interval and the randomization factor. Than,
 * the min is computed as <em>min = current_interval * (1 - factor)</em> and the
 * max is computed as <em>max = = current_interval * (1 + factor)</em>. This way,
 * the initial time interval boundaries grow over time and gives lower probability
 * to generate a time delay colliding with some other client.
 *
 * The interval grows until the max interval limit is reached or until a given
 * (real) time has elapsed. Thus, we are not generate delays indefinitely.
 *
 * @see https://en.wikipedia.org/wiki/Exponential_backoff
 * @see google-http-java-client: com.google.api.client.util.ExponentialBackOff
 */
class ExponentialBackOff : public BackOff {
public:
	typedef Poco::SharedPtr<ExponentialBackOff> Ptr;

	class Config {
	public:
		Config();

		void setInitialInterval(const Poco::Timespan &interval);
		Poco::Timespan initialInterval() const;

		void setMaxInterval(const Poco::Timespan &interval);
		Poco::Timespan maxInterval() const;

		void setMaxElapsedTime(const Poco::Timespan &time);
		Poco::Timespan maxElapsedTime() const;

		void setRandomizationFactor(double factor);
		double randomizationFactor() const;

		void setMultiplier(double multiplier);
		double multiplier() const;

	private:
		Poco::Timespan m_initialInterval;
		Poco::Timespan m_maxInterval;
		Poco::Timespan m_maxElapsedTime;
		double m_randomizationFactor;
		double m_multiplier;
	};

	ExponentialBackOff();

	void applyConfig(const Config &config);
	const Config &config() const;

	Poco::Timespan initialInterval() const;
	Poco::Timespan maxInterval() const;
	Poco::Timespan maxElapsedTime() const;
	double randomizationFactor() const;
	double multiplier() const;

	/**
	 * Return next interval for the next computation.
	 */
	Poco::Timespan nextInterval() const;

	/**
	 * Time elapsed from the last reset or from the initial
	 * creation of the ExponentialBackOff instance.
	 */
	Poco::Timespan elapsed() const;

	Poco::Timespan next() override;
	void reset() override;

protected:
	Poco::Timespan nextRandom();

private:
	Config m_config;

	Poco::Timespan m_nextInterval;
	Poco::Clock m_resetStamp;
	Poco::Random m_random;
};

class ExponentialBackOffFactory : public BackOffFactory {
public:
	BackOff::Ptr create() override;

	void setInitialInterval(const Poco::Timespan &interval);
	void setMaxInterval(const Poco::Timespan &interval);
	void setMaxElapsedTime(const Poco::Timespan &time);
	void setRandomizationFactor(double factor);
	void setMultiplier(double multiplier);

private:
	ExponentialBackOff::Config m_config;
};

}
