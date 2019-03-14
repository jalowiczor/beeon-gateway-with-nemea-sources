#include <vector>

#include <Poco/AtomicCounter.h>
#include <Poco/Event.h>
#include <Poco/Exception.h>
#include <Poco/Thread.h>

#include <cppunit/extensions/HelperMacros.h>

#include "cppunit/BetterAssert.h"
#include "util/ParallelExecutor.h"

#define MAX_WAIT_TIME 10000 // 10 seconds in ms
#define THREADS_COUNT 8

using namespace std;
using namespace Poco;

namespace BeeeOn {

class ParallelExecutorTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ParallelExecutorTest);
	CPPUNIT_TEST(testInvoke);
	CPPUNIT_TEST(testDeferred);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	void tearDown();

	void testInvoke();
	void testDeferred();

private:
	SharedPtr<ParallelExecutor> m_executor;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ParallelExecutorTest);

void ParallelExecutorTest::setUp()
{
	m_executor = new ParallelExecutor;
	m_executor->setMinThreads(1);
	m_executor->setMaxThreads(THREADS_COUNT);
}

void ParallelExecutorTest::tearDown()
{
	m_executor->stop();
	m_executor->flushDeferred();
}

/**
 * Invoke THREADS_COUNT * 4 tasks. Thus, each thread has to
 * process 4 tasks in average. Each task performs:
 *
 * - before[id].wait()
 * - counter[id] = id;
 * - after[id].set()
 *
 * Each task is expected to finish in MAX_WAIT_TIME.
 * There must be no tasks left when the executor is
 * stopped.
 */
void ParallelExecutorTest::testInvoke()
{
	vector<SharedPtr<AtomicCounter>> counters;
	vector<SharedPtr<Event>> before;
	vector<SharedPtr<Event>> after;

	// initialize before, after, counters for each task
	for (int i = 0; i < THREADS_COUNT * 4; ++i) {
		counters.emplace_back(new AtomicCounter{0});
		before.emplace_back(new Event);
		after.emplace_back(new Event);
	}

	// invoke all tasks
	for (int i = 0; i < THREADS_COUNT * 4; ++i) {
		m_executor->invoke([=]() mutable {
			before[i]->wait(MAX_WAIT_TIME);
			*(counters[i]) = i;
			after[i]->set();
		});
	}

	// enable all tasks to proceed
	for (int i = 0; i < THREADS_COUNT * 4; ++i)
		before[i]->set();

	Thread thread;
	thread.start(*m_executor);

	// wait for each task
	for (int i = 0; i < THREADS_COUNT * 4; ++i) {
		CPPUNIT_ASSERT_NO_THROW_MESSAGE(
			"waiting too long for " + to_string(i),
			after[i]->wait(MAX_WAIT_TIME));
		CPPUNIT_ASSERT_EQUAL(i, counters[i]->value());
	}

	m_executor->stop();
	CPPUNIT_ASSERT_NO_THROW(thread.join(MAX_WAIT_TIME));

	// no tasks left
	CPPUNIT_ASSERT_EQUAL(0, m_executor->flushDeferred());
}

/**
 * Run THREADS_COUNT + 5 procedures but do not start the ParallelExecutor
 * thread. In this case, only the THREADS_COUNT procedures is processed.
 */
void ParallelExecutorTest::testDeferred()
{
	CPPUNIT_ASSERT_EQUAL(0, m_executor->flushDeferred());
	SharedPtr<Event> before = new Event(false);
	SharedPtr<Event> after[THREADS_COUNT + 5];

	for (int i = 0; i < THREADS_COUNT + 5; ++i) {
		after[i] = new Event;

		m_executor->invoke([=]() mutable {
			before->wait(MAX_WAIT_TIME);
			after[i]->set();
		});
	}

	before->set();

	for (int i = 0; i < THREADS_COUNT; ++i) {
		CPPUNIT_ASSERT_NO_THROW_MESSAGE(
			"waiting too long for " + to_string(i),
			after[i]->wait(MAX_WAIT_TIME));
	}

	CPPUNIT_ASSERT_EQUAL(5, m_executor->flushDeferred());
}

}
