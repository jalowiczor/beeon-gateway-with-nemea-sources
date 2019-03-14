#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>

#include "cppunit/BetterAssert.h"
#include "util/UnsafePtr.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class UnsafePtrTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(UnsafePtrTest);
	CPPUNIT_TEST(testConstruct);
	CPPUNIT_TEST(testCast);
	CPPUNIT_TEST(testAssign);
	CPPUNIT_TEST(testCall);
	CPPUNIT_TEST(testNull);
	CPPUNIT_TEST(testCompare);
	CPPUNIT_TEST(testClear);
	CPPUNIT_TEST_SUITE_END();
public:
	void testConstruct();
	void testCast();
	void testAssign();
	void testCall();
	void testNull();
	void testCompare();
	void testClear();
};

CPPUNIT_TEST_SUITE_REGISTRATION(UnsafePtrTest);

class UnsafeObject {
public:
	UnsafeObject(const string &name):
		m_name(name)
	{
	}

	virtual ~UnsafeObject()
	{
	}

	string name() const
	{
		return m_name;
	}

private:
	string m_name;
};

class UnsafeObjectChild : public UnsafeObject {
public:
	UnsafeObjectChild(const string &name):
		UnsafeObject(name)
	{
	}
};

void UnsafePtrTest::testConstruct()
{
	UnsafeObject object("object");

	UnsafePtr<UnsafeObject> objectPtr(&object);
	CPPUNIT_ASSERT_EQUAL(&object, objectPtr.get());

	UnsafePtr<UnsafeObject> copyObject(objectPtr);
	CPPUNIT_ASSERT_EQUAL(&object, copyObject.get());
}

void UnsafePtrTest::testCast()
{
	UnsafeObject parent("parent");
	UnsafeObjectChild child("child");

	UnsafePtr<UnsafeObject> parentPtr(&parent);
	UnsafePtr<UnsafeObjectChild> childPtr(&child);
	UnsafePtr<UnsafeObject> childPtrAsParent(childPtr);

	UnsafePtr<UnsafeObject> toParentCast = childPtr;
	CPPUNIT_ASSERT(!toParentCast.isNull());

	// parent cannot cast to child
	CPPUNIT_ASSERT(parentPtr.cast<UnsafeObjectChild>().isNull());
	// child looking like parent can cast to child
	CPPUNIT_ASSERT(!childPtrAsParent.cast<UnsafeObjectChild>().isNull());
}

void UnsafePtrTest::testAssign()
{
	UnsafePtr<UnsafeObject> ptr;

	// simple assign of raw pointer
	UnsafeObject object("object");
	ptr = &object;
	CPPUNIT_ASSERT_EQUAL(&object, ptr.get());

	// assign of raw pointer to derived class
	UnsafeObjectChild child("child");
	ptr = &child;
	CPPUNIT_ASSERT_EQUAL(&child, static_cast<UnsafeObjectChild *>(ptr.get()));

	// simple assign of UnsafePtr<UnsafeObject>
	UnsafePtr<UnsafeObject> assignedPtr = ptr;
	CPPUNIT_ASSERT(!assignedPtr.isNull());
	CPPUNIT_ASSERT_EQUAL(static_cast<UnsafeObject *>(&child), assignedPtr.get());

	// this assign requires explicit cast
	UnsafePtr<UnsafeObjectChild> childPtr = assignedPtr.cast<UnsafeObjectChild>();
	CPPUNIT_ASSERT_EQUAL(&child, childPtr.get());

	// assign UnsafePtr<UnsafeObjectChild> to UnsafePtr<UnsafeObject>
	ptr = childPtr;
	CPPUNIT_ASSERT_EQUAL(&child, static_cast<UnsafeObjectChild *>(ptr.get()));
}

void UnsafePtrTest::testCall()
{
	UnsafeObject parent("parent");
	UnsafeObjectChild child("child");

	UnsafePtr<UnsafeObject> parentPtr(&parent);
	UnsafePtr<UnsafeObject> childPtr(&child);

	CPPUNIT_ASSERT_EQUAL("parent", parentPtr->name());
	CPPUNIT_ASSERT_EQUAL("child", childPtr->name());
}

void UnsafePtrTest::testNull()
{
	UnsafePtr<UnsafeObject> null;

	CPPUNIT_ASSERT(null.isNull());

	CPPUNIT_ASSERT_THROW(
		*null,
		NullPointerException
	);

	CPPUNIT_ASSERT_THROW(
		null->name(),
		NullPointerException
	);

	CPPUNIT_ASSERT(!!null);
}

void UnsafePtrTest::testCompare()
{
	UnsafePtr<UnsafeObject> null;
	UnsafePtr<UnsafeObject> a(reinterpret_cast<UnsafeObject *>(0x1000));
	UnsafePtr<UnsafeObject> b(reinterpret_cast<UnsafeObject *>(0x2000));
	UnsafePtr<UnsafeObject> c(reinterpret_cast<UnsafeObject *>(0x2000));

	CPPUNIT_ASSERT(null < a);
	CPPUNIT_ASSERT(null < b);
	CPPUNIT_ASSERT(a < b);

	CPPUNIT_ASSERT(null <= a);
	CPPUNIT_ASSERT(null <= b);
	CPPUNIT_ASSERT(a <= b);

	CPPUNIT_ASSERT(a > null);
	CPPUNIT_ASSERT(b > null);
	CPPUNIT_ASSERT(b > a);

	CPPUNIT_ASSERT(a >= null);
	CPPUNIT_ASSERT(b >= null);
	CPPUNIT_ASSERT(b >= a);

	CPPUNIT_ASSERT(null == null);
	CPPUNIT_ASSERT(a == a);
	CPPUNIT_ASSERT(b == b);
	CPPUNIT_ASSERT(c == c);
	CPPUNIT_ASSERT(b == c);

	CPPUNIT_ASSERT(a != null);
	CPPUNIT_ASSERT(b != null);
	CPPUNIT_ASSERT(c != null);
	CPPUNIT_ASSERT(a != b);
	CPPUNIT_ASSERT(a != c);
}

void UnsafePtrTest::testClear()
{
	UnsafePtr<int> p;

	CPPUNIT_ASSERT(p.isNull());
	int k = 10;

	p = &k;
	CPPUNIT_ASSERT(!p.isNull());
	CPPUNIT_ASSERT_EQUAL(10, *p);

	p.clear();
	CPPUNIT_ASSERT(p.isNull());
}

}
