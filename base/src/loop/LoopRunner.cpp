#include <string>

#include <Poco/Logger.h>
#include <Poco/Thread.h>

#include "di/Injectable.h"
#include "loop/LoopRunner.h"
#include "util/ClassInfo.h"
#include "util/ThreadNamer.h"

using namespace std;
using namespace BeeeOn;
using namespace Poco;

BEEEON_OBJECT_BEGIN(BeeeOn, LoopRunner)
BEEEON_OBJECT_CASTABLE(StoppableLoop)
BEEEON_OBJECT_PROPERTY("runnables", &LoopRunner::addRunnable)
BEEEON_OBJECT_PROPERTY("loops", &LoopRunner::addLoop)
BEEEON_OBJECT_PROPERTY("autoStart", &LoopRunner::setAutoStart)
BEEEON_OBJECT_PROPERTY("stopParallel", &LoopRunner::setStopParallel)
BEEEON_OBJECT_HOOK("done", &LoopRunner::autoStart)
BEEEON_OBJECT_END(BeeeOn, LoopRunner)

static string repr(const StoppableLoop::Ptr loop)
{
	SharedPtr<StoppableLoopAdapter> adapter = loop.cast<StoppableLoopAdapter>();

	if (adapter.isNull())
		return ClassInfo::repr(loop.get());
	else
		return ClassInfo::repr(adapter->runnable().get());
}

LoopRunner::LoopRunner():
	m_autoStart(false),
	m_stopParallel(false)
{
}

LoopRunner::~LoopRunner()
{
	stop();
}

void LoopRunner::addRunnable(StoppableRunnable::Ptr runnable)
{
	addLoop(new StoppableLoopAdapter(runnable));
}

void LoopRunner::addLoop(StoppableLoop::Ptr loop)
{
	FastMutex::ScopedLock guard(m_lock);

	m_loops.push_back(loop);
}

void LoopRunner::setAutoStart(bool enable)
{
	m_autoStart = enable;
}

void LoopRunner::setStopParallel(bool parallel)
{
	m_stopParallel = parallel;
}

void LoopRunner::stop()
{
	if (m_started.empty())
		return;

	logger().notice("stopping "
			+ to_string(m_started.size())
			+ " loops"
			+ (m_stopParallel? " (parallel)" : ""),
			__FILE__, __LINE__);

	FastMutex::ScopedLock guard(m_lock);

	stopAll(m_started);
}

void LoopRunner::stopAll(list<Stopper> &list)
{
	if (m_stopParallel) {
		try {
			stopParallel(list);
		}
		BEEEON_CATCH_CHAIN(logger())
	}

	while (!list.empty()) {
		list.back().run();
		list.pop_back();
	}
}

void LoopRunner::stopParallel(list<Stopper> &loops)
{
	list<Stopper> failed;
	vector<Thread> threads(loops.size());
	size_t i = 0;

	for (auto it = loops.rbegin(); it != loops.rend(); ++it) {
		try {
			threads[i++].start(*it);
		}
		BEEEON_CATCH_CHAIN_ACTION(logger(),
			failed.emplace_back(*it))
	}

	for (auto &thread : threads) {
		try {
			thread.join();
		}
		BEEEON_CATCH_CHAIN(logger())
	}

	if (!failed.empty()) {
		logger().warning(
			"fallback to signle-thread stop for "
			+ to_string(failed.size()) + " loops",
			__FILE__, __LINE__);
	}

	loops.clear();
	failed.splice(failed.begin(), loops);
}

void LoopRunner::start()
{
	FastMutex::ScopedLock guard(m_lock);

	for (auto &loop : m_loops) {
		try {
			if (logger().debug())
				logger().debug("start " + repr(loop), __FILE__, __LINE__);

			loop->start();
			m_started.push_back(Stopper(loop));
		}
		BEEEON_CATCH_CHAIN_ACTION_RETHROW(logger(), stopAll(m_started));
	}

	logger().notice("started " + to_string(m_started.size()) + " loops",
			__FILE__, __LINE__);
}

void LoopRunner::autoStart()
{
	if (m_autoStart) {
		logger().information("auto-starting loops", __FILE__, __LINE__);
		start();
	}
}

LoopRunner::Stopper::Stopper():
	Loggable(typeid(LoopRunner))
{
}

LoopRunner::Stopper::Stopper(StoppableLoop::Ptr loop):
	Loggable(typeid(LoopRunner)),
	m_loop(loop)
{
}

void LoopRunner::Stopper::run()
{
	ThreadNamer namer("stopper-of-" + repr(m_loop));

	if (m_loop.isNull())
		return;

	if (logger().debug())
		logger().debug("stop " + repr(m_loop), __FILE__, __LINE__);

	try {
		m_loop->stop();

		if (logger().debug())
			logger().debug("stopped " + repr(m_loop), __FILE__, __LINE__);
	}
	BEEEON_CATCH_CHAIN(logger())
}
