#include <cppunit/extensions/HelperMacros.h>

#include "util/ThreadWrapperAsyncWork.h"

using namespace Poco;
using namespace std;

namespace BeeeOn {

class ThreadWrapperAsyncWorkTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ThreadWrapperAsyncWorkTest);
	CPPUNIT_TEST(testJoin);
	CPPUNIT_TEST(testCancel);
	CPPUNIT_TEST_SUITE_END();
public:
	void testJoin();
	void testCancel();

protected:
	void runFast();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ThreadWrapperAsyncWorkTest);

void ThreadWrapperAsyncWorkTest::testJoin()
{
	Thread thread;
	ThreadWrapperAsyncWork<> work(thread);

	thread.startFunc([]() {});

	CPPUNIT_ASSERT(work.tryJoin(10 * Timespan::SECONDS));
	CPPUNIT_ASSERT(!thread.isRunning());
}

void ThreadWrapperAsyncWorkTest::testCancel()
{
	Thread thread;
	ThreadWrapperAsyncWork<> work(thread);

	thread.startFunc([]() {});

	CPPUNIT_ASSERT_NO_THROW(work.cancel());
	CPPUNIT_ASSERT(!thread.isRunning());
}

}
