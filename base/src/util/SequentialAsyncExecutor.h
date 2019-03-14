#pragma once

#include <queue>

#include <Poco/AtomicCounter.h>
#include <Poco/Event.h>
#include <Poco/Mutex.h>
#include <Poco/Timespan.h>

#include "loop/StoppableRunnable.h"
#include "util/AsyncExecutor.h"
#include "util/Loggable.h"

namespace BeeeOn {

class SequentialAsyncExecutor :
	public StoppableRunnable,
	public AsyncExecutor,
	Loggable {
public:
	typedef Poco::SharedPtr<SequentialAsyncExecutor> Ptr;

	SequentialAsyncExecutor();
	~SequentialAsyncExecutor();

	void setStopTimeout(const Poco::Timespan &timeout);

	void invoke(std::function<void()> f) override;

	void run() override;
	void stop() override;

protected:
	void finalize();

private:
	void execute(std::function<void()> task);
	std::queue<std::function<void()>> m_taskQueue;
	Poco::Event m_wakeupEvent;
	Poco::FastMutex m_queueMutex;
	Poco::AtomicCounter m_stopRequested;
	Poco::Event m_stoppedEvent;
	Poco::Timespan m_stopTimeout;
};

}
