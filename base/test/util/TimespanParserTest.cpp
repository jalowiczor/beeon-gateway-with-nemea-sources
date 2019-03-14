#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>

#include "cppunit/BetterAssert.h"
#include "util/TimespanParser.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class TimespanParserTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(TimespanParserTest);
	CPPUNIT_TEST(testTryValid);
	CPPUNIT_TEST(testTryInvalid);
	CPPUNIT_TEST(testNoThrow);
	CPPUNIT_TEST(testThrow);
	CPPUNIT_TEST_SUITE_END();
public:
	void testTryValid();
	void testTryInvalid();
	void testNoThrow();
	void testThrow();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TimespanParserTest);

void TimespanParserTest::testTryValid()
{
	Timespan result = 0;

	CPPUNIT_ASSERT(result == 0);

	CPPUNIT_ASSERT(TimespanParser::tryParse("441 d", result));
	CPPUNIT_ASSERT(result == 441 * Timespan::DAYS);

	CPPUNIT_ASSERT(TimespanParser::tryParse("15 h", result));
	CPPUNIT_ASSERT(result == 15 * Timespan::HOURS);

	CPPUNIT_ASSERT(TimespanParser::tryParse("90 m", result));
	CPPUNIT_ASSERT(result == 90 * Timespan::MINUTES);

	CPPUNIT_ASSERT(TimespanParser::tryParse("10 s", result));
	CPPUNIT_ASSERT(result == 10 * Timespan::SECONDS);

	CPPUNIT_ASSERT(TimespanParser::tryParse("4332 ms", result));
	CPPUNIT_ASSERT(result == 4332 * Timespan::MILLISECONDS);

	CPPUNIT_ASSERT(TimespanParser::tryParse("12190 us", result));
	CPPUNIT_ASSERT(result == 12190);

	CPPUNIT_ASSERT(TimespanParser::tryParse("0", result));
	CPPUNIT_ASSERT(result == 0);

	CPPUNIT_ASSERT(TimespanParser::tryParse("-56 d", result));
	CPPUNIT_ASSERT(result == -56 * Timespan::DAYS);

	CPPUNIT_ASSERT(TimespanParser::tryParse("-98 h", result));
	CPPUNIT_ASSERT(result == -98 * Timespan::HOURS);

	CPPUNIT_ASSERT(TimespanParser::tryParse("-73 m", result));
	CPPUNIT_ASSERT(result == -73 * Timespan::MINUTES);

	CPPUNIT_ASSERT(TimespanParser::tryParse("-1 s", result));
	CPPUNIT_ASSERT(result == -1 * Timespan::SECONDS);

	CPPUNIT_ASSERT(TimespanParser::tryParse("-632 ms", result));
	CPPUNIT_ASSERT(result == -632 * Timespan::MILLISECONDS);

	CPPUNIT_ASSERT(TimespanParser::tryParse("-901 us", result));
	CPPUNIT_ASSERT(result == -901);

	CPPUNIT_ASSERT(TimespanParser::tryParse("-9", result));
	CPPUNIT_ASSERT(result == -9);

	CPPUNIT_ASSERT(TimespanParser::tryParse("0015", result));
	CPPUNIT_ASSERT(result == 15);
}

void TimespanParserTest::testTryInvalid()
{
	Timespan result = 0;

	CPPUNIT_ASSERT(!TimespanParser::tryParse("foeiere", result));
	CPPUNIT_ASSERT(result == 0);

	CPPUNIT_ASSERT(!TimespanParser::tryParse("---", result));
	CPPUNIT_ASSERT(result == 0);

	CPPUNIT_ASSERT(!TimespanParser::tryParse("0x10", result));
	CPPUNIT_ASSERT(result == 0);

	CPPUNIT_ASSERT(!TimespanParser::tryParse("16d", result));
	CPPUNIT_ASSERT(result == 0);

	CPPUNIT_ASSERT(!TimespanParser::tryParse("16h", result));
	CPPUNIT_ASSERT(result == 0);

	CPPUNIT_ASSERT(!TimespanParser::tryParse("16m", result));
	CPPUNIT_ASSERT(result == 0);

	CPPUNIT_ASSERT(!TimespanParser::tryParse("16s", result));
	CPPUNIT_ASSERT(result == 0);

	CPPUNIT_ASSERT(!TimespanParser::tryParse("16ms", result));
	CPPUNIT_ASSERT(result == 0);

	CPPUNIT_ASSERT(!TimespanParser::tryParse("16us", result));
	CPPUNIT_ASSERT(result == 0);
}

void TimespanParserTest::testNoThrow()
{
	CPPUNIT_ASSERT(
		TimespanParser::parse("1 d") == 1 * Timespan::DAYS);

	CPPUNIT_ASSERT(
		TimespanParser::parse("5 h") == 5 * Timespan::HOURS);

	CPPUNIT_ASSERT(
		TimespanParser::parse("78 m") == 78 * Timespan::MINUTES);

	CPPUNIT_ASSERT(
		TimespanParser::parse("1209 s") == 1209 * Timespan::SECONDS);

	CPPUNIT_ASSERT(
		TimespanParser::parse("329 ms") == 329 * Timespan::MILLISECONDS);

	CPPUNIT_ASSERT(
		TimespanParser::parse("1290 us") == 1290);

	CPPUNIT_ASSERT(
		TimespanParser::parse("10") == 10);

	CPPUNIT_ASSERT(
		TimespanParser::parse("-1 d") == -1 * Timespan::DAYS);

	CPPUNIT_ASSERT(
		TimespanParser::parse("-5 h") == -5 * Timespan::HOURS);

	CPPUNIT_ASSERT(
		TimespanParser::parse("-78 m") == -78 * Timespan::MINUTES);

	CPPUNIT_ASSERT(
		TimespanParser::parse("-1209 s") == -1209 * Timespan::SECONDS);

	CPPUNIT_ASSERT(
		TimespanParser::parse("-329 ms") == -329 * Timespan::MILLISECONDS);

	CPPUNIT_ASSERT(
		TimespanParser::parse("-1290 us") == -1290);

	CPPUNIT_ASSERT(
		TimespanParser::parse("-10") == -10);
}

void TimespanParserTest::testThrow()
{
	CPPUNIT_ASSERT_THROW(
		TimespanParser::parse("dfvfdfd"),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		TimespanParser::parse("0x0343"),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		TimespanParser::parse("12s"),
		SyntaxException
	);

	CPPUNIT_ASSERT_THROW(
		TimespanParser::parse("15343h"),
		SyntaxException
	);
}

}
