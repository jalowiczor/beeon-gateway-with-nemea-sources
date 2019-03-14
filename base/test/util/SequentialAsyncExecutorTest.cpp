#include <Poco/Event.h>
#include <Poco/Exception.h>
#include <Poco/Thread.h>

#include <exception>

#include <cppunit/extensions/HelperMacros.h>

#include "util/SequentialAsyncExecutor.h"

#define MAX_WAIT_TIME 10000 // 10 seconds in ms

using namespace std;
using namespace Poco;

namespace BeeeOn {

class SequentialAsyncExecutorTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(SequentialAsyncExecutorTest);
	CPPUNIT_TEST(testRunEmpty);
	CPPUNIT_TEST(testRunTwice);
	CPPUNIT_TEST(testOneTask);
	CPPUNIT_TEST(testOneThenRun);
	CPPUNIT_TEST(testThousandTasks);
	CPPUNIT_TEST(testThousandTasksThenRun);
	CPPUNIT_TEST_SUITE_END();
public:
	void testRunEmpty();
	void testRunTwice();
	void testOneTask();
	void testOneThenRun();
	void testThousandTasks();
	void testThousandTasksThenRun();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SequentialAsyncExecutorTest);

/**
 * testRunEmpty - create, start and stop executor instance without
 * inserting any tasks to find any potential memory leaks using valgrind tool.
 */
void SequentialAsyncExecutorTest::testRunEmpty()
{
	Thread t;
	SequentialAsyncExecutor executor;
	t.start(executor);
	executor.stop();
	t.join();
}

/**
 * testRunTwice - create then start and stop executor twice without
 * inserting any tasks to find any potential memory leaks using valgrind tool.
 */
void SequentialAsyncExecutorTest::testRunTwice()
{
	Thread t;
	SequentialAsyncExecutor executor;
	t.start(executor);
	executor.stop();
	t.join();

	t.start(executor);
	executor.stop();
	t.join();
}

/**
 * testOneTask - create and start executor instance. Invoke it with one task and
 * wait for it to finish.
 * If task takes more then MAX_WAIT_TIME to finish its considered as failure.
 */
void SequentialAsyncExecutorTest::testOneTask()
{
	Thread t;
	SequentialAsyncExecutor executor;
	Event finishEvent;

	t.start(executor);

	executor.invoke([&finishEvent]() {finishEvent.set();});

	CPPUNIT_ASSERT(finishEvent.tryWait(MAX_WAIT_TIME));

	executor.stop();
	t.join();
}

/**
 * testOneThenRun - create executor instance, invoke it with one task then start its thread.
 * Task should be able to finish within given time.
 * If task takes more then MAX_WAIT_TIME to finish its considered as failure.
 */
void SequentialAsyncExecutorTest::testOneThenRun()
{
	Thread t;
	SequentialAsyncExecutor executor;
	Event finishEvent;

	executor.invoke([&finishEvent]() {finishEvent.set();});

	t.start(executor);

	CPPUNIT_ASSERT(finishEvent.tryWait(MAX_WAIT_TIME));

	executor.stop();
	t.join();
}

/**
 * testThousandTasks - create executor instance and start it.
 * Then invoke it thousand times and wait for tasks to finish.
 * If task takes more then MAX_WAIT_TIME to finish its considered as failure.
 */
void SequentialAsyncExecutorTest::testThousandTasks()
{
	Thread t;
	SequentialAsyncExecutor executor;
	Event finishEvent;
	int results[1000];

	t.start(executor);

	for(int i=0; i<1000; i++){
		executor.invoke([&results, i]() {results[i] = i;});
	}
	executor.invoke([&finishEvent]() {finishEvent.set();});

	CPPUNIT_ASSERT(finishEvent.tryWait(MAX_WAIT_TIME));

	for(int i=0; i<1000; i++)
		CPPUNIT_ASSERT(results[i] == i);

	executor.stop();
	t.join();
}

/**
 * testThousandTasks - create executor instance then invoke it thousand times.
 * After invoke-ing start its thread and wait for tasks to finish.
 * If task takes more then MAX_WAIT_TIME to finish its considered as failure.
 */
void SequentialAsyncExecutorTest::testThousandTasksThenRun()
{
	Thread t;
	SequentialAsyncExecutor executor;
	Event finishEvent;
	int results[1000];

	for (int i=0; i<1000; i++){
		results[i] = 0;
		executor.invoke([&results, i]() {results[i] = i;});
	}

	for (int i=0; i<1000; i++) {
		CPPUNIT_ASSERT(results[i] == 0);
	}

	executor.invoke([&finishEvent]() {finishEvent.set();});

	t.start(executor);

	CPPUNIT_ASSERT(finishEvent.tryWait(MAX_WAIT_TIME));

	for(int i=0; i<1000; i++)
		CPPUNIT_ASSERT(results[i] == i);

	executor.stop();
	t.join();
}

} // namespace BeeeOn
