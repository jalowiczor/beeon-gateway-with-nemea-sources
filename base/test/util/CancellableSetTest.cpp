#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>

#include "cppunit/BetterAssert.h"
#include "util/CancellableSet.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class CancellableSetTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(CancellableSetTest);
	CPPUNIT_TEST(testManageUnmanage);
	CPPUNIT_TEST(testCancel);
	CPPUNIT_TEST_SUITE_END();
public:
	void testManageUnmanage();
	void testCancel();
};

CPPUNIT_TEST_SUITE_REGISTRATION(CancellableSetTest);

class FakeCancellable : public Cancellable {
public:
	typedef SharedPtr<FakeCancellable> Ptr;

	void cancel() override
	{
		cancelled = true;
	}

	bool cancelled = false;
};

void CancellableSetTest::testManageUnmanage()
{
	CancellableSet set;

	FakeCancellable::Ptr one = new FakeCancellable;
	FakeCancellable::Ptr two = new FakeCancellable;
	FakeCancellable::Ptr three = new FakeCancellable;

	CPPUNIT_ASSERT(!set.unmanage(one));
	CPPUNIT_ASSERT(!set.unmanage(two));
	CPPUNIT_ASSERT(!set.unmanage(three));

	CPPUNIT_ASSERT(!one->cancelled);
	CPPUNIT_ASSERT(!two->cancelled);
	CPPUNIT_ASSERT(!three->cancelled);

	set.manage(one);
	set.manage(two);
	set.manage(three);

	CPPUNIT_ASSERT(!one->cancelled);
	CPPUNIT_ASSERT(!two->cancelled);
	CPPUNIT_ASSERT(!three->cancelled);

	CPPUNIT_ASSERT(set.unmanage(one));
	CPPUNIT_ASSERT(!set.unmanage(one));
	CPPUNIT_ASSERT(set.unmanage(two));
	CPPUNIT_ASSERT(!set.unmanage(two));
	CPPUNIT_ASSERT(set.unmanage(three));
	CPPUNIT_ASSERT(!set.unmanage(three));

	CPPUNIT_ASSERT(!one->cancelled);
	CPPUNIT_ASSERT(!two->cancelled);
	CPPUNIT_ASSERT(!three->cancelled);

	set.cancel();

	CPPUNIT_ASSERT(!one->cancelled);
	CPPUNIT_ASSERT(!two->cancelled);
	CPPUNIT_ASSERT(!three->cancelled);
}

void CancellableSetTest::testCancel()
{
	CancellableSet set;

	FakeCancellable::Ptr one = new FakeCancellable;
	FakeCancellable::Ptr two = new FakeCancellable;
	FakeCancellable::Ptr three = new FakeCancellable;

	CPPUNIT_ASSERT(!set.unmanage(one));
	CPPUNIT_ASSERT(!set.unmanage(two));
	CPPUNIT_ASSERT(!set.unmanage(three));

	CPPUNIT_ASSERT(!one->cancelled);
	CPPUNIT_ASSERT(!two->cancelled);
	CPPUNIT_ASSERT(!three->cancelled);

	set.manage(one);
	set.manage(two);
	set.manage(three);

	CPPUNIT_ASSERT(!one->cancelled);
	CPPUNIT_ASSERT(!two->cancelled);
	CPPUNIT_ASSERT(!three->cancelled);

	set.cancel();

	CPPUNIT_ASSERT(one->cancelled);
	CPPUNIT_ASSERT(two->cancelled);
	CPPUNIT_ASSERT(three->cancelled);

	// all were unmanaged by cancel
	CPPUNIT_ASSERT(!set.unmanage(one));
	CPPUNIT_ASSERT(!set.unmanage(two));
	CPPUNIT_ASSERT(!set.unmanage(three));
}

}
