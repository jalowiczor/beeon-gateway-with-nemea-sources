#include <Poco/Exception.h>
#include <Poco/Logger.h>
#include <Poco/Thread.h>

#include "di/Injectable.h"
#include "util/ParallelExecutor.h"
#include "util/Occasionally.h"
#include "util/ThreadNamer.h"

BEEEON_OBJECT_BEGIN(BeeeOn, ParallelExecutor)
BEEEON_OBJECT_CASTABLE(AsyncExecutor)
BEEEON_OBJECT_CASTABLE(StoppableRunnable)
BEEEON_OBJECT_PROPERTY("minThreads", &ParallelExecutor::setMinThreads)
BEEEON_OBJECT_PROPERTY("maxThreads", &ParallelExecutor::setMaxThreads)
BEEEON_OBJECT_PROPERTY("threadIdleTime", &ParallelExecutor::setThreadIdleTime)
BEEEON_OBJECT_PROPERTY("baseName", &ParallelExecutor::setBaseName)
BEEEON_OBJECT_PROPERTY("contentedDelay", &ParallelExecutor::setContentedDelay)
BEEEON_OBJECT_END(BeeeOn, ParallelExecutor)

using namespace std;
using namespace Poco;
using namespace BeeeOn;

ParallelExecutor::ThreadLambda::ThreadLambda(
		function<void()> f,
		Event &event,
		const string &baseName):
	m_func(f),
	m_event(event),
	m_baseName(baseName)
{
}

void ParallelExecutor::ThreadLambda::start(ThreadPool &pool)
{
	ThreadLambda::Ptr self(this, true); // assure ref >= 1

	pool.start(*this);

	/*
	 * The lambda-wrapper will release itself after execution
	 * and thus we must duplicate it here when we are sure that
	 * its thread has started.
	 */
	this->duplicate(); // assure ref > 1, we can return
}

void ParallelExecutor::ThreadLambda::run()
{
	ThreadLambda::Ptr self(this, true); // assure ref > 1
	this->release(); // drop to ref >= 1

	ThreadNamer namer(m_baseName + to_string(Thread::currentTid()));

	try {
		m_func();
	}
	BEEEON_CATCH_CHAIN(logger())

	m_event.set();
}

ParallelExecutor::ParallelExecutor():
	m_baseName("parallel-"),
	m_contentedDelay(100 * Timespan::MILLISECONDS),
	m_stop(false)
{
}

ParallelExecutor::~ParallelExecutor()
{
	(void) flushDeferred();
}

void ParallelExecutor::setBaseName(const string &baseName)
{
	m_baseName = baseName;
}

void ParallelExecutor::setContentedDelay(const Timespan &delay)
{
	if (delay.totalMilliseconds() < 1) {
		throw InvalidArgumentException(
			"contentedDelay must be at least 1 ms");
	}

	m_contentedDelay = delay;
}

void ParallelExecutor::run()
{
	static Occasionally occasionally;

	while (!m_stop) {
		ScopedLockWithUnlock<FastMutex> guard(m_lock);

		if (m_stop)
			break;

		if (m_queue.empty()) {
			guard.unlock();
			m_event.wait();
			continue;
		}

		occasionally.execute([this]() {
			logger().notice(
				m_baseName + "main "
				"allocated: %d, "
				"available: %d, "
				"capacity: %d, "
				"used: %d, ",
				pool().allocated(),
				pool().available(),
				pool().capacity(),
				pool().used());
		});

		ThreadLambda::Ptr tl = m_queue.front();
		try {
			tl->start(pool());
			m_queue.pop();
		}
		catch (const NoThreadAvailableException &) {
			// when contented, insert a delay to give chance
			// to threads to finish their work
			guard.unlock();
			m_event.tryWait(m_contentedDelay.totalMilliseconds());
			continue;
		}
	}

	m_stop = false;
}

void ParallelExecutor::stop()
{
	FastMutex::ScopedLock guard(m_lock);

	m_stop = true;
	m_event.set();
	pool().stopAll();
}

void ParallelExecutor::defer(ThreadLambda::Ptr tl)
{
	m_queue.emplace(tl);
	m_event.set();
}

void ParallelExecutor::startOrDefer(ThreadLambda::Ptr tl)
{
	if (!m_queue.empty()) {
		defer(tl);
		return;
	}

	try {
		tl->start(pool());
	}
	catch (const NoThreadAvailableException &) {
		defer(tl);
	}
}

size_t ParallelExecutor::flushDeferred()
{
	FastMutex::ScopedLock guard(m_lock);

	const size_t count = m_queue.size();

	if (count > 0) {
		logger().warning(
			"flushing "
			+ to_string(count)
			+ " deferred procedures",
			__FILE__, __LINE__);
	}

	while (!m_queue.empty())
		m_queue.pop();

	return count;
}

void ParallelExecutor::invoke(function<void()> f)
{
	FastMutex::ScopedLock guard(m_lock);

	ThreadLambda::Ptr tl = new ThreadLambda(f, m_event, m_baseName);
	startOrDefer(tl);
}
