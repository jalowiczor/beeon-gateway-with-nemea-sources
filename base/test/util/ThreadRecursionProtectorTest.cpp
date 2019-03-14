#include <vector>

#include <cppunit/extensions/HelperMacros.h>

#include <Poco/AtomicCounter.h>
#include <Poco/Exception.h>
#include <Poco/SharedPtr.h>
#include <Poco/Thread.h>

#include "cppunit/BetterAssert.h"
#include "util/ThreadRecursionProtector.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class ThreadRecursionProtectorTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ThreadRecursionProtectorTest);
	CPPUNIT_TEST(testEnterLeave);
	CPPUNIT_TEST(testEnterLeaveMultithreaded);
	CPPUNIT_TEST(testRecursionDetected);
	CPPUNIT_TEST_SUITE_END();
public:
	void testEnterLeave();
	void testEnterLeaveMultithreaded();
	void testRecursionDetected();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ThreadRecursionProtectorTest);

void ThreadRecursionProtectorTest::testEnterLeave()
{
	ThreadRecursionProtector protector;

	CPPUNIT_ASSERT_NO_THROW(protector.enter());
	protector.leave();
}

void ThreadRecursionProtectorTest::testEnterLeaveMultithreaded()
{
	ThreadRecursionProtector protector;

	vector<SharedPtr<Thread>> threads;
	vector<AtomicCounter> dones;

	for (int i = 0; i < 100; ++i) {
		threads.emplace_back(new Thread);
		dones.emplace_back(0);
	}

	for (int i = 0; i < 100; ++i) {
		threads[i]->startFunc([&protector, &dones, i]() {
			try {
				protector.enter();
				dones[i] = 1;
			}
			catch (...) {
				dones[i] = 2;
				return;
			}

			protector.leave();
			dones[i] = 3;
		});
	}

	for (auto thread : threads)
		thread->join();

	for (int i = 0; i < 100; ++i) {
		CPPUNIT_ASSERT_MESSAGE(
			"thread " + to_string(i) + " finished with " + to_string(dones[i]),
			dones[i] == 3);
	}
}

void ThreadRecursionProtectorTest::testRecursionDetected()
{
	ThreadRecursionProtector protector;

	CPPUNIT_ASSERT_NO_THROW(protector.enter());
	CPPUNIT_ASSERT_THROW(protector.enter(), IllegalStateException);
}

}
