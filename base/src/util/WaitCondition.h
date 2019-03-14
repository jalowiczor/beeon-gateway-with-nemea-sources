#pragma once

#include <Poco/AtomicCounter.h>
#include <Poco/Condition.h>
#include <Poco/Mutex.h>
#include <Poco/Timespan.h>

namespace BeeeOn {

/**
 * @brief WaitCondition works as a barrier that waits until
 * some condition is met. When the condition is met and
 * broadcasted, all waiting threads are woken up.
 *
 * The WaitCondition can be configured to be non-repetitive.
 * I.e. after the first broadcast, all calls to wait or tryWait
 * would immediatelly return as successful.
 */
class WaitCondition {
public:
	WaitCondition(bool repeat = true);
	~WaitCondition();

	/**
	 * @brief Broadcaster can be used to ensure that the broadcast
	 * is called even in cast when an unexpected exception is thrown.
	 * Broadcaster calls broadcast() from its destructor unless it
	 * has been called explicitly.
	 */
	class Broadcaster {
	public:
		Broadcaster(WaitCondition &condition);

		/**
		 * @brief It calls broadcast() internally to make sure
		 * that the given wait condition has been broadcasted.
		 */
		~Broadcaster();

		/**
		 * @brief Call WaitCondition::broadcast() unless it has
		 * already been called.
		 */
		void broadcast();

	private:
		bool m_broadcasted;
		WaitCondition &m_condition;
	};

	/**
	 * @brief Wait until the condition is broadcasted.
	 * Negative timeout would block infinitly (until broadcasted).
	 *
	 * @throws Poco::TimeoutException when timeout is exceeded
	 */
	void wait(const Poco::Timespan &timeout = -1);

	/**
	 * @brief Wait until the condition is broadcasted.
	 * Negative timeout would block infinitly (until broadcasted).
	 *
	 * @returns true if woken by broadcast, false when timeout has exceeded
	 */
	bool tryWait(const Poco::Timespan &timeout = -1);

	/**
	 * @brief Broadcast that the condition has been met.
	 */
	void broadcast();

protected:
	/**
	 * @brief Convert the given timeout into milliseconds.
	 * Negative value is converted to -1. Timeout shorter
	 * then 1 ms is converted to 1 ms.
	 */
	long toMilliseconds(const Poco::Timespan &timeout) const;

	/**
	 * @brief The actual waiting logic.
	 */
	bool doWait(long ms);

private:
	Poco::FastMutex m_lock;
	Poco::Condition m_condition;
	bool m_canRepeat;
	Poco::AtomicCounter m_broadcasted;
};

}
