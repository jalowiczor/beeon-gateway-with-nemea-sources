#pragma once

#include <list>

#include <Poco/Mutex.h>
#include <Poco/SharedPtr.h>

#include "loop/StoppableRunnable.h"
#include "loop/StoppableLoop.h"
#include "util/Loggable.h"

namespace BeeeOn {

class LoopRunner : public StoppableLoop, public Loggable {
public:
	typedef Poco::SharedPtr<LoopRunner> Ptr;

	LoopRunner();
	~LoopRunner();

	void addRunnable(StoppableRunnable::Ptr runnable);
	void addLoop(StoppableLoop::Ptr loop);
	void setAutoStart(bool enable);

	/**
	 * @brief Set whether stopAll() should run in parallel.
	 */
	void setStopParallel(bool parallel);

	void start() override;
	void stop() override;
	void autoStart();

protected:
	/**
	 * @brief Wrapper around StoppableLoop that allows to
	 * stop it from inside a thread.
	 */
	class Stopper : public Poco::Runnable, Loggable {
	public:
		Stopper();
		Stopper(StoppableLoop::Ptr loop);

		void run() override;

	private:
		StoppableLoop::Ptr m_loop;
	};

	/**
	 * @brief Stop all loop in reverse order. If the property stopParallel
	 * is true then the loops are stopped in parallel (multiple threads).
	 *
	 * If there is an issue with starting threads, the loops that were not
	 * stopped in parallel are stopped sequentially as a fallback.
	 */
	void stopAll(std::list<Stopper> &list);

	/**
	 * @brief Stops the given list of loops in parallel. Loops that have been
	 * stopped are removed from the list.
	 */
	void stopParallel(std::list<Stopper> &list);

private:
	bool m_autoStart;
	bool m_stopParallel;
	Poco::FastMutex m_lock;
	std::list<StoppableLoop::Ptr> m_loops;
	std::list<Stopper> m_started;
};

}
