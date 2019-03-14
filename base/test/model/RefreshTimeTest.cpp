#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>

#include "cppunit/BetterAssert.h"
#include "model/RefreshTime.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class RefreshTimeTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(RefreshTimeTest);
	CPPUNIT_TEST(testNone);
	CPPUNIT_TEST(testDisabled);
	CPPUNIT_TEST(testFromSeconds);
	CPPUNIT_TEST(testFromMinutes);
	CPPUNIT_TEST(testParse);
	CPPUNIT_TEST(testToString);
	CPPUNIT_TEST(testCompare);
	CPPUNIT_TEST_SUITE_END();
public:
	void testNone();
	void testDisabled();
	void testFromSeconds();
	void testFromMinutes();
	void testParse();
	void testToString();
	void testCompare();
};

CPPUNIT_TEST_SUITE_REGISTRATION(RefreshTimeTest);

void RefreshTimeTest::testNone()
{
	CPPUNIT_ASSERT(RefreshTime::NONE.isNone());
	CPPUNIT_ASSERT(!RefreshTime::NONE.isDisabled());
	CPPUNIT_ASSERT_EQUAL(RefreshTime::NONE, RefreshTime::NONE);
	CPPUNIT_ASSERT_EQUAL(-1, RefreshTime::NONE.seconds());
}

void RefreshTimeTest::testDisabled()
{
	CPPUNIT_ASSERT(RefreshTime::DISABLED.isDisabled());
	CPPUNIT_ASSERT(!RefreshTime::DISABLED.isNone());
	CPPUNIT_ASSERT_EQUAL(RefreshTime::DISABLED, RefreshTime::DISABLED);
	CPPUNIT_ASSERT_EQUAL(0, RefreshTime::DISABLED.seconds());
}

void RefreshTimeTest::testFromSeconds()
{
	CPPUNIT_ASSERT_EQUAL(RefreshTime::DISABLED, RefreshTime::fromSeconds(0));
	CPPUNIT_ASSERT_EQUAL(RefreshTime::NONE, RefreshTime::fromSeconds(-1));
	CPPUNIT_ASSERT_EQUAL(RefreshTime::NONE, RefreshTime::fromSeconds(-454123));

	CPPUNIT_ASSERT_EQUAL(  -1, RefreshTime::fromSeconds(-1).seconds());
	CPPUNIT_ASSERT_EQUAL(  -1, RefreshTime::fromSeconds(-90732).seconds());
	CPPUNIT_ASSERT_EQUAL(   0, RefreshTime::fromSeconds(0).seconds());
	CPPUNIT_ASSERT_EQUAL(   1, RefreshTime::fromSeconds(1).seconds());
	CPPUNIT_ASSERT_EQUAL(   5, RefreshTime::fromSeconds(5).seconds());
	CPPUNIT_ASSERT_EQUAL(  10, RefreshTime::fromSeconds(10).seconds());
	CPPUNIT_ASSERT_EQUAL(  15, RefreshTime::fromSeconds(15).seconds());
	CPPUNIT_ASSERT_EQUAL(  60, RefreshTime::fromSeconds(60).seconds());
	CPPUNIT_ASSERT_EQUAL(1000, RefreshTime::fromSeconds(1000).seconds());
}

void RefreshTimeTest::testFromMinutes()
{
	CPPUNIT_ASSERT_EQUAL(RefreshTime::DISABLED, RefreshTime::fromMinutes(0));
	CPPUNIT_ASSERT_EQUAL(RefreshTime::NONE, RefreshTime::fromMinutes(-1));
	CPPUNIT_ASSERT_EQUAL(RefreshTime::NONE, RefreshTime::fromMinutes(-454123));

	CPPUNIT_ASSERT_EQUAL(  -1, RefreshTime::fromMinutes(-1).seconds());
	CPPUNIT_ASSERT_EQUAL(  -1, RefreshTime::fromMinutes(-90732).seconds());
	CPPUNIT_ASSERT_EQUAL(   0, RefreshTime::fromMinutes(0).seconds());
	CPPUNIT_ASSERT_EQUAL(  60, RefreshTime::fromMinutes(1).seconds());
	CPPUNIT_ASSERT_EQUAL( 300, RefreshTime::fromMinutes(5).seconds());
	CPPUNIT_ASSERT_EQUAL( 600, RefreshTime::fromMinutes(10).seconds());
	CPPUNIT_ASSERT_EQUAL( 900, RefreshTime::fromMinutes(15).seconds());
	CPPUNIT_ASSERT_EQUAL(3600, RefreshTime::fromMinutes(60).seconds());
}

void RefreshTimeTest::testParse()
{
	CPPUNIT_ASSERT_EQUAL(RefreshTime::DISABLED, RefreshTime::parse("disabled"));
	CPPUNIT_ASSERT_EQUAL(RefreshTime::DISABLED, RefreshTime::parse("0"));

	CPPUNIT_ASSERT_EQUAL(RefreshTime::NONE, RefreshTime::parse("none"));
	CPPUNIT_ASSERT_EQUAL(RefreshTime::NONE, RefreshTime::parse("-1"));
	CPPUNIT_ASSERT_EQUAL(RefreshTime::NONE, RefreshTime::parse("-10123"));

	CPPUNIT_ASSERT_EQUAL(  -1, RefreshTime::parse("-1").seconds());
	CPPUNIT_ASSERT_EQUAL(  -1, RefreshTime::parse("-39863").seconds());
	CPPUNIT_ASSERT_EQUAL(   0, RefreshTime::parse("0").seconds());
	CPPUNIT_ASSERT_EQUAL(   1, RefreshTime::parse("1").seconds());
	CPPUNIT_ASSERT_EQUAL(   5, RefreshTime::parse("5").seconds());
	CPPUNIT_ASSERT_EQUAL(1000, RefreshTime::parse("1000").seconds());
}

void RefreshTimeTest::testToString()
{
	CPPUNIT_ASSERT_EQUAL("none", RefreshTime::NONE.toString());
	CPPUNIT_ASSERT_EQUAL("disabled", RefreshTime::DISABLED.toString());
	CPPUNIT_ASSERT_EQUAL("none", RefreshTime::fromSeconds(-1).toString());
	CPPUNIT_ASSERT_EQUAL("disabled", RefreshTime::fromSeconds(0).toString());
	CPPUNIT_ASSERT_EQUAL("1", RefreshTime::fromSeconds(1).toString());
	CPPUNIT_ASSERT_EQUAL("156", RefreshTime::fromSeconds(156).toString());
	CPPUNIT_ASSERT_EQUAL("18098", RefreshTime::fromSeconds(18098).toString());
}

void RefreshTimeTest::testCompare()
{
	CPPUNIT_ASSERT(RefreshTime::NONE <= RefreshTime::NONE);
	CPPUNIT_ASSERT(RefreshTime::NONE >= RefreshTime::NONE);

	CPPUNIT_ASSERT(RefreshTime::DISABLED <= RefreshTime::DISABLED);
	CPPUNIT_ASSERT(RefreshTime::DISABLED >= RefreshTime::DISABLED);

	CPPUNIT_ASSERT(RefreshTime::fromSeconds(10) <= RefreshTime::fromSeconds(10));
	CPPUNIT_ASSERT(RefreshTime::fromSeconds(10) >= RefreshTime::fromSeconds(10));

	CPPUNIT_ASSERT(RefreshTime::NONE < RefreshTime::DISABLED);
	CPPUNIT_ASSERT(RefreshTime::NONE <= RefreshTime::DISABLED);
	CPPUNIT_ASSERT(RefreshTime::DISABLED > RefreshTime::NONE);
	CPPUNIT_ASSERT(RefreshTime::DISABLED >= RefreshTime::NONE);

	CPPUNIT_ASSERT(RefreshTime::NONE < RefreshTime::fromSeconds(1));
	CPPUNIT_ASSERT(RefreshTime::NONE <= RefreshTime::fromSeconds(1));
	CPPUNIT_ASSERT(RefreshTime::fromSeconds(1) > RefreshTime::NONE);
	CPPUNIT_ASSERT(RefreshTime::fromSeconds(1) >= RefreshTime::NONE);

	CPPUNIT_ASSERT(RefreshTime::DISABLED < RefreshTime::fromSeconds(1));
	CPPUNIT_ASSERT(RefreshTime::DISABLED <= RefreshTime::fromSeconds(1));
	CPPUNIT_ASSERT(RefreshTime::fromSeconds(1) > RefreshTime::DISABLED);
	CPPUNIT_ASSERT(RefreshTime::fromSeconds(1) >= RefreshTime::DISABLED);

	CPPUNIT_ASSERT(RefreshTime::fromSeconds(1) < RefreshTime::fromSeconds(10));
	CPPUNIT_ASSERT(RefreshTime::fromSeconds(1) <= RefreshTime::fromSeconds(10));
	CPPUNIT_ASSERT(RefreshTime::fromSeconds(10) > RefreshTime::fromSeconds(1));
	CPPUNIT_ASSERT(RefreshTime::fromSeconds(10) >= RefreshTime::fromSeconds(1));
}

}
