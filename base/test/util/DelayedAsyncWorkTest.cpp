#include <cppunit/extensions/HelperMacros.h>

#include "util/DelayedAsyncWork.h"

using namespace Poco;
using namespace std;

namespace BeeeOn {

class DelayedAsyncWorkTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(DelayedAsyncWorkTest);
	CPPUNIT_TEST(testRunDelayed);
	CPPUNIT_TEST(testCancelBeforeRun);
	CPPUNIT_TEST_SUITE_END();
public:
	void testRunDelayed();
	void testCancelBeforeRun();
};

CPPUNIT_TEST_SUITE_REGISTRATION(DelayedAsyncWorkTest);

void DelayedAsyncWorkTest::testRunDelayed()
{
	Poco::Event event;

	DelayedAsyncWork<> work([&](DelayedAsyncWork<> &) {event.set();}, 100 * Timespan::MILLISECONDS);

	CPPUNIT_ASSERT(event.tryWait(10 * Timespan::SECONDS));
	CPPUNIT_ASSERT(work.tryJoin(10 * Timespan::SECONDS));
}

void DelayedAsyncWorkTest::testCancelBeforeRun()
{
	bool run = false;
	bool cancel = false;
	
	DelayedAsyncWork<> work(
		[&](DelayedAsyncWork<> &) {run = true;},
		[&](DelayedAsyncWork<> &) {cancel = true;},
		10 * Timespan::MINUTES);

	work.cancel();

	CPPUNIT_ASSERT(!run);
	CPPUNIT_ASSERT(cancel);
}

}
