#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>

#include "cppunit/BetterAssert.h"
#include "loop/StopControl.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class StopControlTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(StopControlTest);
	CPPUNIT_TEST(testShouldStop);
	CPPUNIT_TEST(testRun);
	CPPUNIT_TEST_SUITE_END();
public:
	void testShouldStop();
	void testRun();
};

CPPUNIT_TEST_SUITE_REGISTRATION(StopControlTest);

void StopControlTest::testShouldStop()
{
	StopControl control;

	CPPUNIT_ASSERT(!control.shouldStop());

	control.requestStop();
	CPPUNIT_ASSERT(control.shouldStop());

	control.clear();
	CPPUNIT_ASSERT(!control.shouldStop());

	control.requestWakeup();
	CPPUNIT_ASSERT(!control.shouldStop());

	control.requestStop();
	CPPUNIT_ASSERT(control.shouldStop());
}

void StopControlTest::testRun()
{
	StopControl control;

	CPPUNIT_ASSERT(!control.shouldStop());
	control.requestStop();

	{
		StopControl::Run run(control);
		CPPUNIT_ASSERT(!!run);
		CPPUNIT_ASSERT(!control.shouldStop());

		control.requestStop();

		CPPUNIT_ASSERT(!run);
		CPPUNIT_ASSERT(control.shouldStop());
	}

	// constructor clears stop flag
	CPPUNIT_ASSERT(!control.shouldStop());
}

}
