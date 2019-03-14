#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>

#include "cppunit/BetterAssert.h"
#include "util/RandomBackOff.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class RandomBackOffTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(RandomBackOffTest);
	CPPUNIT_TEST(testNext);
	CPPUNIT_TEST(testRandomNext);
	CPPUNIT_TEST_SUITE_END();
public:
	void testNext();
	void testRandomNext();
};

CPPUNIT_TEST_SUITE_REGISTRATION(RandomBackOffTest);

void RandomBackOffTest::testNext()
{
	const Timespan MIN = 100 * Timespan::MILLISECONDS;
	const Timespan MAX = 500 * Timespan::MILLISECONDS;
	const size_t COUNT = 20000;
	const uint32_t SEED = 0;

	RandomBackOff backoff(MIN, MAX, COUNT, SEED);

	Timespan delay;
	size_t total = 0;

	while ((delay = backoff.next()) != RandomBackOff::STOP) {
		CPPUNIT_ASSERT(delay >= MIN);
		CPPUNIT_ASSERT(delay <= MAX);
		++total;
	}

	CPPUNIT_ASSERT_EQUAL(COUNT, total);
}

void RandomBackOffTest::testRandomNext()
{
	const Timespan MIN = 100 * Timespan::MILLISECONDS;
	const Timespan MAX = 500 * Timespan::MILLISECONDS;
	const size_t COUNT = 20000;

	RandomBackOff backoff(MIN, MAX, COUNT, RandomBackOff::RANDOM_SEED);

	Timespan delay;
	size_t total = 0;

	while ((delay = backoff.next()) != RandomBackOff::STOP) {
		CPPUNIT_ASSERT(delay >= MIN);
		CPPUNIT_ASSERT(delay <= MAX);
		++total;
	}

	CPPUNIT_ASSERT_EQUAL(COUNT, total);
}

}
