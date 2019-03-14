#pragma once

#include <queue>
#include <string>

#include <Poco/AtomicCounter.h>
#include <Poco/AutoPtr.h>
#include <Poco/Event.h>
#include <Poco/Mutex.h>
#include <Poco/RefCountedObject.h>
#include <Poco/Timespan.h>

#include "loop/StoppableLoop.h"
#include "util/AsyncExecutor.h"
#include "util/HavingThreadPool.h"

namespace BeeeOn {

/**
 * @brief Implementation of AsyncExecutor interface that invokes
 * given procedures in parallel.
 *
 * ParallelExecutor maintains a thread-pool which is used to allow
 * parallel execution of the procedure invocations requests. If
 * the executor is contented, the requests are enqueued to not loose
 * any of them.
 */
class ParallelExecutor :
	public AsyncExecutor,
	public StoppableRunnable,
	public HavingThreadPool {
public:
	ParallelExecutor();
	~ParallelExecutor();

	typedef Poco::SharedPtr<ParallelExecutor> Ptr;

	void setBaseName(const std::string &baseName);
	void setContentedDelay(const Poco::Timespan &delay);

	/**
	 * Executes a loop that invokes deferred procedures.
	 */
	void run() override;

	/**
	 * Stops the loop and waits until all threads are stopped.
	 */
	void stop() override;

	/**
	 * Get rid of all defered lambda-wrappers.
	 * It is called automatically by destructor.
	 *
	 * @returns number of flushed deferred procedures.
	 */
	size_t flushDeferred();

	void invoke(std::function<void()> f) override;

private:
	/**
	 * @brief Wrapper around the procedure to be invoked.
	 */
	class ThreadLambda :
		public Poco::Runnable,
		public Poco::RefCountedObject,
		Loggable {
	public:
		typedef Poco::AutoPtr<ThreadLambda> Ptr;

		ThreadLambda(
			std::function<void()> f,
			Poco::Event &event,
			const std::string &baseName);

		void start(Poco::ThreadPool &pool);
		void run() override;

	private:
		std::function<void()> m_func;
		Poco::Event &m_event;
		std::string m_baseName;
	};

	/**
	 * Defer execution of the given lambda-wrapper.
	 * The lambda is appended to the waiting queue.
	 * The m_lock must be held when calling this method.
	 */
	void defer(ThreadLambda::Ptr tl);

	/**
	 * Start the given lambda-wrapper if the waiting queue
	 * is empty and there is an available thread. Otherwise,
	 * defer execution.
	 * The m_lock must be held when calling this method.
	 */
	void startOrDefer(ThreadLambda::Ptr tl);

private:
	std::string m_baseName;
	Poco::Timespan m_contentedDelay;
	std::queue<ThreadLambda::Ptr> m_queue;
	Poco::AtomicCounter m_stop;
	Poco::FastMutex m_lock;
	Poco::Event m_event;
};

}
