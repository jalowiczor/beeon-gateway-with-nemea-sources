#include <cppunit/extensions/HelperMacros.h>

#include "util/BlockingAsyncWork.h"

using namespace Poco;
using namespace std;

namespace BeeeOn {

class BlockingAsyncWorkTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(BlockingAsyncWorkTest);
	CPPUNIT_TEST(testVoidInstanceIsSingleton);
	CPPUNIT_TEST(testNonVoidInstanceIsNotSingleton);
	CPPUNIT_TEST(testJoinAlwaysTrue);
	CPPUNIT_TEST_SUITE_END();
public:
	void testVoidInstanceIsSingleton();
	void testNonVoidInstanceIsNotSingleton();
	void testJoinAlwaysTrue();
};

CPPUNIT_TEST_SUITE_REGISTRATION(BlockingAsyncWorkTest);

void BlockingAsyncWorkTest::testVoidInstanceIsSingleton()
{
	CPPUNIT_ASSERT(BlockingAsyncWork<>::instance() == BlockingAsyncWork<>::instance());
}

void BlockingAsyncWorkTest::testNonVoidInstanceIsNotSingleton()
{
	CPPUNIT_ASSERT(BlockingAsyncWork<int>::instance() != BlockingAsyncWork<int>::instance());
}

void BlockingAsyncWorkTest::testJoinAlwaysTrue()
{
	BlockingAsyncWork<> work;

	CPPUNIT_ASSERT(work.tryJoin(1000));
}

}
