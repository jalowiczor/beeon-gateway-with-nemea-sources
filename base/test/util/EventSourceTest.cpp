#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Event.h>
#include <Poco/Logger.h>
#include <Poco/Thread.h>

#include "cppunit/BetterAssert.h"
#include "util/EventSource.h"
#include "util/SequentialAsyncExecutor.h"

using namespace Poco;
using namespace std;

namespace BeeeOn {

class EventSourceTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(EventSourceTest);
	CPPUNIT_TEST(testFireEvent);
	CPPUNIT_TEST_SUITE_END();
public:
	void testFireEvent();
};

CPPUNIT_TEST_SUITE_REGISTRATION(EventSourceTest);

struct TestingESListener {
	typedef SharedPtr<TestingESListener> Ptr;

	void onFirst(int n)
	{
		m_firstValue = n;
		m_event.set();
	}

	void onSecond(const string &s)
	{
		m_secondValue = s;
		m_event.set();
	}

	Event m_event;
	int m_firstValue = 0;
	string m_secondValue;
};

void EventSourceTest::testFireEvent()
{
	SharedPtr<TestingESListener> listener = new TestingESListener;
	SequentialAsyncExecutor::Ptr executor = new SequentialAsyncExecutor;
	Thread thread;

	thread.start(*executor);

	EventSource<TestingESListener> source;
	source.addListener(listener);
	source.setAsyncExecutor(executor);

	source.fireEvent(5, &TestingESListener::onFirst);
	CPPUNIT_ASSERT(listener->m_event.tryWait(5000));

	source.fireEvent("test", &TestingESListener::onSecond);
	CPPUNIT_ASSERT(listener->m_event.tryWait(5000));

	executor->stop();
	CPPUNIT_ASSERT(thread.tryJoin(5000));

	CPPUNIT_ASSERT_EQUAL(5, listener->m_firstValue);
	CPPUNIT_ASSERT_EQUAL("test", listener->m_secondValue);
}

}
