#include <cppunit/extensions/HelperMacros.h>

#include "util/AbstractAsyncWork.h"

using namespace Poco;
using namespace std;

namespace BeeeOn {

class AbstractAsyncWorkTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(AbstractAsyncWorkTest);
	CPPUNIT_TEST(testVoidResult);
	CPPUNIT_TEST(testNonVoidSetResultTwice);
	CPPUNIT_TEST_SUITE_END();
public:
	void testVoidResult();
	void testNonVoidSetResultTwice();
};

CPPUNIT_TEST_SUITE_REGISTRATION(AbstractAsyncWorkTest);

template <typename Result = Void>
class TestableAbstractAsyncWork : public AbstractAsyncWork<Result> {
public:
	bool tryJoin(const Timespan &) override
	{
		return true;
	}

	void cancel() override
	{
	}
};

void AbstractAsyncWorkTest::testVoidResult()
{
	TestableAbstractAsyncWork<> work;
	Void v;

	CPPUNIT_ASSERT_THROW(work.setResult(v), NotImplementedException);
	CPPUNIT_ASSERT(work.result().isNull());
}

void AbstractAsyncWorkTest::testNonVoidSetResultTwice()
{
	TestableAbstractAsyncWork<int> work;

	CPPUNIT_ASSERT(work.result().isNull());

	CPPUNIT_ASSERT_NO_THROW(work.setResult(5));
	CPPUNIT_ASSERT_EQUAL(5, work.result().value());

	CPPUNIT_ASSERT_THROW(work.setResult(6), IllegalStateException);
	CPPUNIT_ASSERT_EQUAL(5, work.result().value());
}

}
