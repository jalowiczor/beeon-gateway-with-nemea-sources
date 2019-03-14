#pragma once

#include <functional>

#include <Poco/Timer.h>
#include <Poco/Timespan.h>

#include "util/Loggable.h"

namespace BeeeOn {

/**
 * @brief Executes some function periodacally based on the given interval.
 *
 * The class simplifies coding of simple periodical tasks like generation
 * of statistics, sampling data, etc.
 */
class PeriodicRunner : Poco::Timer, protected Loggable {
public:
	typedef std::function<void()> Callback;

	PeriodicRunner();
	virtual ~PeriodicRunner();

	/**
	 * Starts the runner to executed repeatedly the given callback.
	 */
	void start(const Callback &callback);

	/**
	 * Stops the runner.
	 */
	void stop();

	/**
	 * Set interval of executions.
	 */
	void setInterval(const Poco::Timespan &interval);

private:
	/**
	 * Executes the callback. This is called periodically by the timer.
	 */
	void onStart(Poco::Timer &timer);

private:
	Poco::TimerCallback<PeriodicRunner> m_invoke;
	Callback m_callback;
	Poco::Timespan m_interval;
};

}
