#pragma once

#include <Poco/Condition.h>
#include <Poco/Mutex.h>
#include <Poco/Timespan.h>

namespace BeeeOn {

/**
 * @brief StopControl implements the stop mechanism generically.
 */
class StopControl {
public:
	/**
	 * @brief Helper class for managing a common stoppable loop
	 * situation:
	 * <pre>
	 * void run()
	 * {
	 *    StopControl::Run run(m_stopControl);
	 *
	 *    while (run) {
	 *       ...
	 *       run.waitStoppable(...)
	 *       ...
	 *    }
	 * }
	 *
	 * void stop()
	 * {
	 *    m_stopControl.requestStop();
	 * }
	 * </pre>
	 */
	class Run {
	public:
		Run(StopControl &control);
		~Run();

		/**
		 * @brief Delegates to StopControl::waitStoppable().
		 */
		bool waitStoppable(const Poco::Timespan &timeout);

		/**
		 * @brief Equivalent to !StopControl::shouldStop().
		 */
		operator bool() const;

	private:
		StopControl &m_control;
	};

	StopControl();

	/**
	 * Wait for the specified timeout unless a stop is requested.
	 * If the timeout is negative, it waits without timeout.
	 * Timeout in range 0 < timeout < 1 ms is fixed to be 1 ms.
	 *
	 * @returns true when woken up, false when timeout exceeded
	 */
	bool waitStoppable(const Poco::Timespan &timeout);

	/**
	 * @returns true to determine whether a stop request has
	 * been made.
	 */
	bool shouldStop() const;

	/**
	 * Request stop. The shouldStop() call would return true
	 * until clear() is called.
	 */
	void requestStop();

	/**
	 * Request wakeup (without requesting a stop).
	 */
	void requestWakeup();

	/**
	 * Clear the stop request and reinitialize.
	 */
	void clear();

protected:
	/**
	 * Perform the actual m_event.wait() or m_event.tryWait(ms).
	 */
	bool doWait(long ms);

private:
	bool m_stop;
	mutable Poco::FastMutex m_lock;
	Poco::Condition m_condition;
};

}
