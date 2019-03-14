#pragma once

#include <Poco/Thread.h>
#include <Poco/Timespan.h>

namespace BeeeOn {

/**
 * @brief Joiner implements join() on a thread that can be called
 * multiple times from different threads. The point is that the
 * Poco::Thread::join() MUST NOT be called twice (unless its timeout
 * exceeds).
 *
 * The Joiner creates a queue of joining threads via FastMutex. All
 * the threads are waiting on the mutex except one (in FIFO order)
 * that performs the actual join(). If this single join succeeds, all
 * other threads would return successfully. If the single one join
 * does not succeeds (due to timeout), the next thread in the queue
 * performs the actual join(), etc.
 */
class Joiner {
public:
	Joiner(Poco::Thread &thread);

	/**
	 * @brief Join the underlying thread. The timeout controls
	 * whether we are waiting indefinitely (when negative) or
	 * we can be interrupted by timeout.
	 *
	 * @throws Poco::TimeoutException when the timeout exceeds
	 */
	void join(const Poco::Timespan &timeout = -1);

	/**
	 * @brief Join the underlying thread. The timeout controls
	 * whether we are waiting indefinitely (when negative) or
	 * we can be interrupted by timeout.
	 *
	 * @returns true if the join has succeeded, false on timeout
	 */
	bool tryJoin(const Poco::Timespan &timeout = -1);

protected:
	/**
 	 * @brief Implement locking to allow exactly 1 thread to
 	 * call the join(). The lock respects the given timeout.
 	 */
	void joinFromMany(long ms);

	/**
 	 * @brief Call the actual join based on the timeout.
 	 */
	void doJoin(long ms);

private:
	Poco::Thread &m_thread;
	Poco::FastMutex m_joinLock;
	bool m_joined;
};

}
