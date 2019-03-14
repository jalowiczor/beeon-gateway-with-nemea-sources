#include <Poco/Clock.h>
#include <Poco/Logger.h>
#include <Poco/ScopedLock.h>

#include "di/Injectable.h"
#include "util/SequentialAsyncExecutor.h"
#include "util/ThreadNamer.h"

BEEEON_OBJECT_BEGIN(BeeeOn, SequentialAsyncExecutor)
BEEEON_OBJECT_CASTABLE(AsyncExecutor)
BEEEON_OBJECT_CASTABLE(StoppableRunnable)
BEEEON_OBJECT_PROPERTY("stopTimeout", &SequentialAsyncExecutor::setStopTimeout)
BEEEON_OBJECT_END(BeeeOn, SequentialAsyncExecutor)

using namespace std;
using namespace Poco;
using namespace BeeeOn;

SequentialAsyncExecutor::SequentialAsyncExecutor() :
	m_stopRequested(false),
	m_stopTimeout(0)
{
}

SequentialAsyncExecutor::~SequentialAsyncExecutor()
{
	if (!m_taskQueue.empty()) {
		poco_warning(logger(),
			"deleting object with non empty queue, tasks count: "
			+ to_string(m_taskQueue.size()));
	}
}

void SequentialAsyncExecutor::setStopTimeout(const Timespan &timeout)
{
	if (timeout > 0 && timeout < 1 * Timespan::MILLISECONDS)
		throw InvalidArgumentException("stopTimeout must be at least 1 ms");

	if (timeout < 0)
		m_stopTimeout = -1;
	else
		m_stopTimeout = timeout;
}

void SequentialAsyncExecutor::invoke(std::function<void()> f)
{
	FastMutex::ScopedLock lock(m_queueMutex);
	m_taskQueue.push(f);
	m_wakeupEvent.set();
}

void SequentialAsyncExecutor::run()
{
	ThreadNamer namer("sequential-executor");

	std::function<void()> task;

	while (!m_stopRequested) {
		ScopedLockWithUnlock<FastMutex> guard(m_queueMutex);

		if (m_taskQueue.empty()) {
			guard.unlock();
			m_wakeupEvent.wait();
			continue;
		}

		task = m_taskQueue.front();
		m_taskQueue.pop();

		guard.unlock();
		execute(task);
	}

	ScopedLock<FastMutex> guard(m_queueMutex);

	if (m_stopTimeout != 0)
		finalize();

	if (!m_taskQueue.empty()) {
		poco_warning(logger(), "exiting thread with non empty queue");
	}

	m_stopRequested = false;
	m_stoppedEvent.set();
}

void SequentialAsyncExecutor::finalize()
{
	const Clock started;
	std::function<void()> task;

	while (!m_taskQueue.empty()) {
		task = m_taskQueue.front();
		m_taskQueue.pop();
		execute(task);

		Timespan remaining = m_stopTimeout - started.elapsed();
		if (remaining < 1 * Timespan::MILLISECONDS)
			remaining = 1 * Timespan::MILLISECONDS;

		if (m_stopTimeout >= 0 && remaining <= 0)
			break;
	}
}

void SequentialAsyncExecutor::execute(std::function<void()> task)
{
	try {
		task();
	}
	BEEEON_CATCH_CHAIN(logger())
}

void SequentialAsyncExecutor::stop()
{
	m_stopRequested = true;
	m_wakeupEvent.set();

	m_stoppedEvent.wait();
}
