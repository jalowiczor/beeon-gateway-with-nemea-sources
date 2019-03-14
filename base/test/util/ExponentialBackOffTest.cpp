#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>

#include "cppunit/BetterAssert.h"
#include "util/ExponentialBackOff.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class ExponentialBackOffTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ExponentialBackOffTest);
	CPPUNIT_TEST(testDefaultSettings);
	CPPUNIT_TEST_SUITE_END();
public:
	void testDefaultSettings();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ExponentialBackOffTest);

/**
 * Test that the ExponentialBackOff works as expected with its default settings.
 * @see google-http-java-client: com.google.api.client.util.ExponentialBackOff
 */
void ExponentialBackOffTest::testDefaultSettings()
{
	ExponentialBackOff backOff;
	Timespan value;

	CPPUNIT_ASSERT_EQUAL(500L, backOff.nextInterval().totalMilliseconds());

	value = backOff.next();
	CPPUNIT_ASSERT(250L <= value.totalMilliseconds());
	CPPUNIT_ASSERT(750L >= value.totalMilliseconds());

	CPPUNIT_ASSERT_EQUAL(750L, backOff.nextInterval().totalMilliseconds());

	value = backOff.next();
	CPPUNIT_ASSERT( 375L <= value.totalMilliseconds());
	CPPUNIT_ASSERT(1125L >= value.totalMilliseconds());

	CPPUNIT_ASSERT_EQUAL(1125L, backOff.nextInterval().totalMilliseconds());

	value = backOff.next();
	CPPUNIT_ASSERT( 562L <= value.totalMilliseconds());
	CPPUNIT_ASSERT(1687L >= value.totalMilliseconds());

	CPPUNIT_ASSERT_EQUAL(1687L, backOff.nextInterval().totalMilliseconds());

	value = backOff.next();
	CPPUNIT_ASSERT( 843L <= value.totalMilliseconds());
	CPPUNIT_ASSERT(2530L >= value.totalMilliseconds());

	CPPUNIT_ASSERT_EQUAL(2530L, backOff.nextInterval().totalMilliseconds());

	value = backOff.next();
	CPPUNIT_ASSERT(1265L <= value.totalMilliseconds());
	CPPUNIT_ASSERT(3795L >= value.totalMilliseconds());

	CPPUNIT_ASSERT_EQUAL(3795L, backOff.nextInterval().totalMilliseconds());

	value = backOff.next();
	CPPUNIT_ASSERT(1897L <= value.totalMilliseconds());
	CPPUNIT_ASSERT(5692L >= value.totalMilliseconds());

	CPPUNIT_ASSERT_EQUAL(5692L, backOff.nextInterval().totalMilliseconds());

	value = backOff.next();
	CPPUNIT_ASSERT(2846L <= value.totalMilliseconds());
	CPPUNIT_ASSERT(8538L >= value.totalMilliseconds());

	CPPUNIT_ASSERT_EQUAL(8538L, backOff.nextInterval().totalMilliseconds());

	value = backOff.next();
	CPPUNIT_ASSERT( 4269L <= value.totalMilliseconds());
	CPPUNIT_ASSERT(12807L >= value.totalMilliseconds());

	CPPUNIT_ASSERT_EQUAL(12807L, backOff.nextInterval().totalMilliseconds());

	value = backOff.next();
	CPPUNIT_ASSERT( 6403L <= value.totalMilliseconds());
	CPPUNIT_ASSERT(19210L >= value.totalMilliseconds());

	CPPUNIT_ASSERT_EQUAL(19210L, backOff.nextInterval().totalMilliseconds());

	// let it stop by setting small max elapsed time
	ExponentialBackOff::Config config = backOff.config();
	config.setMaxElapsedTime(backOff.elapsed());
	backOff.applyConfig(config);

	value = backOff.next();
	CPPUNIT_ASSERT_EQUAL(
		ExponentialBackOff::STOP.totalMicroseconds(),
		value.totalMicroseconds());
}

}
