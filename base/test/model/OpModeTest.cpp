#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>

#include "cppunit/BetterAssert.h"
#include "model/OpMode.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class OpModeTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(OpModeTest);
	CPPUNIT_TEST(testParse);
	CPPUNIT_TEST(testParseInvalid);
	CPPUNIT_TEST(testToString);
	CPPUNIT_TEST_SUITE_END();
public:
	void testParse();
	void testParseInvalid();
	void testToString();
};

CPPUNIT_TEST_SUITE_REGISTRATION(OpModeTest);

void OpModeTest::testParse()
{
	CPPUNIT_ASSERT_EQUAL(
		OpMode::TRY_ONCE,
		OpMode::parse("try_once").raw());

	CPPUNIT_ASSERT_EQUAL(
		OpMode::TRY_HARDER,
		OpMode::parse("try_harder").raw());

	CPPUNIT_ASSERT_EQUAL(
		OpMode::REPEAT_ON_FAIL,
		OpMode::parse("repeat_on_fail").raw());
}

void OpModeTest::testParseInvalid()
{
	CPPUNIT_ASSERT_THROW(
		OpMode::parse("something"),
		InvalidArgumentException);
}

void OpModeTest::testToString()
{
	CPPUNIT_ASSERT_EQUAL(
		"try_once",
		OpMode(OpMode::TRY_ONCE).toString());

	CPPUNIT_ASSERT_EQUAL(
		"try_harder",
		OpMode(OpMode::TRY_HARDER).toString());

	CPPUNIT_ASSERT_EQUAL(
		"repeat_on_fail",
		OpMode(OpMode::REPEAT_ON_FAIL).toString());
}

}
