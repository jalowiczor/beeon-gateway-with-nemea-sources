#include <cppunit/extensions/HelperMacros.h>

#include "cppunit/BetterAssert.h"

#include "Poco/Timespan.h"

#include "util/FailDetector.h"

using namespace std;
using namespace BeeeOn;
using namespace Poco;

namespace BeeeOn {

class FailDetectorTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(FailDetectorTest);
	CPPUNIT_TEST(testFail);
	CPPUNIT_TEST(testSuccess);
	CPPUNIT_TEST(testTresholdUnlimited);
	CPPUNIT_TEST(testLastFailBefore);
	CPPUNIT_TEST_SUITE_END();

public:
	void testFail();
	void testSuccess();
	void testTresholdUnlimited();
	void testLastFailBefore();

};

CPPUNIT_TEST_SUITE_REGISTRATION(FailDetectorTest);

void FailDetectorTest::testFail()
{
	FailDetector detector(2);

	CPPUNIT_ASSERT(!detector.isFailed());
	CPPUNIT_ASSERT(!detector.isGoingToFail());

	detector.fail();

	CPPUNIT_ASSERT(!detector.isFailed());
	CPPUNIT_ASSERT(detector.isGoingToFail());

	detector.fail();

	CPPUNIT_ASSERT(detector.isFailed());
	CPPUNIT_ASSERT(detector.isGoingToFail());
}

void FailDetectorTest::testSuccess()
{
	FailDetector detector(1);

	CPPUNIT_ASSERT(!detector.isFailed());
	CPPUNIT_ASSERT(!detector.isGoingToFail());

	detector.fail();

	CPPUNIT_ASSERT(detector.isFailed());
	CPPUNIT_ASSERT(detector.isGoingToFail());

	detector.success();

	CPPUNIT_ASSERT(!detector.isFailed());
	CPPUNIT_ASSERT(!detector.isGoingToFail());
}

void FailDetectorTest::testTresholdUnlimited()
{
	FailDetector detector(FailDetector::TRESHOLD_UNLIMITED);

	CPPUNIT_ASSERT(!detector.isFailed());

	for (int i = 0; i < 100; ++i)
		detector.fail();

	CPPUNIT_ASSERT(!detector.isFailed());
	CPPUNIT_ASSERT(detector.isGoingToFail());
}

void FailDetectorTest::testLastFailBefore()
{
	FailDetector detector(1);

	CPPUNIT_ASSERT(!detector.isFailed());

	detector.fail();

	CPPUNIT_ASSERT(detector.isFailed());

	// we assume that time passed since fail is (much) smaller than 5s.
	CPPUNIT_ASSERT(!detector.lastFailBefore(5 * Timespan::SECONDS));

	// we assume that time passed since fail is bigger than 0s.
	CPPUNIT_ASSERT(detector.lastFailBefore(0 * Timespan::SECONDS));
}

}
