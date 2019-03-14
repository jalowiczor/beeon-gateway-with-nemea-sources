#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>
#include <Poco/Timespan.h>
#include <Poco/Timestamp.h>

#include "cppunit/BetterAssert.h"
#include "util/TimeInterval.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class TimeIntervalTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(TimeIntervalTest);
	CPPUNIT_TEST(testConstruct);
	CPPUNIT_TEST(testConstructInvalid);
	CPPUNIT_TEST(testPast);
	CPPUNIT_TEST_SUITE_END();
public:
	void testConstruct();
	void testConstructInvalid();
	void testPast();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TimeIntervalTest);

void TimeIntervalTest::testConstruct()
{
	TimeInterval interval0(0, 0);

	CPPUNIT_ASSERT(interval0.isEmpty());
	CPPUNIT_ASSERT_EQUAL(0, interval0.start().epochMicroseconds());
	CPPUNIT_ASSERT_EQUAL(0, interval0.end().epochMicroseconds());

	TimeInterval interval1(0, 10);

	CPPUNIT_ASSERT(!interval1.isEmpty());
	CPPUNIT_ASSERT_EQUAL(0, interval1.start().epochMicroseconds());
	CPPUNIT_ASSERT_EQUAL(10, interval1.end().epochMicroseconds());

	Timestamp now;

	TimeInterval interval2(now, now);

	CPPUNIT_ASSERT(interval2.isEmpty());
	CPPUNIT_ASSERT_EQUAL(now.epochMicroseconds(), interval2.start().epochMicroseconds());
	CPPUNIT_ASSERT_EQUAL(now.epochMicroseconds(), interval2.end().epochMicroseconds());

	TimeInterval interval3(now, now + 10 * Timespan::HOURS);

	CPPUNIT_ASSERT(!interval3.isEmpty());
	CPPUNIT_ASSERT_EQUAL(now.epochMicroseconds(), interval3.start().epochMicroseconds());
	CPPUNIT_ASSERT_EQUAL(now.epochMicroseconds() + 10 * Timespan::HOURS, interval3.end().epochMicroseconds());
}

void TimeIntervalTest::testConstructInvalid()
{
	CPPUNIT_ASSERT_THROW(
		TimeInterval interval(10, 9),
		InvalidArgumentException
	);

	Timestamp now;

	CPPUNIT_ASSERT_THROW(
		TimeInterval interval(now, now - 1),
		InvalidArgumentException
	);
}

void TimeIntervalTest::testPast()
{
	Timestamp now;

	TimeInterval interval0 = TimeInterval::past(0, now);

	CPPUNIT_ASSERT(interval0.isEmpty());
	CPPUNIT_ASSERT_EQUAL(now.epochMicroseconds(), interval0.start().epochMicroseconds());
	CPPUNIT_ASSERT_EQUAL(now.epochMicroseconds(), interval0.end().epochMicroseconds());

	TimeInterval interval1 = TimeInterval::past(2 * Timespan::HOURS, now);

	CPPUNIT_ASSERT(!interval1.isEmpty());
	CPPUNIT_ASSERT_EQUAL(now.epochMicroseconds() - 2 * Timespan::HOURS, interval1.start().epochMicroseconds());
	CPPUNIT_ASSERT_EQUAL(now.epochMicroseconds(), interval1.end().epochMicroseconds());
}

}
