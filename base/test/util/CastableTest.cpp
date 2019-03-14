#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>

#include "cppunit/BetterAssert.h"
#include "util/Castable.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class CastableTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(CastableTest);
	CPPUNIT_TEST(testIs);
	CPPUNIT_TEST(testConstIs);
	CPPUNIT_TEST(testCast);
	CPPUNIT_TEST(testConstCast);
	CPPUNIT_TEST_SUITE_END();
public:
	void testIs();
	void testConstIs();
	void testCast();
	void testConstCast();
};

CPPUNIT_TEST_SUITE_REGISTRATION(CastableTest);

class CastableA : public Castable {
};

class CastableB : public CastableA {
};

class CastableC : public Castable {
};

void CastableTest::testIs()
{
	CastableA a;
	CastableB b;
	CastableC c;

	CPPUNIT_ASSERT(a.is<CastableA>());
	CPPUNIT_ASSERT(!a.is<CastableB>());
	CPPUNIT_ASSERT(!a.is<CastableC>());

	CPPUNIT_ASSERT(b.is<CastableA>());
	CPPUNIT_ASSERT(b.is<CastableB>());
	CPPUNIT_ASSERT(!b.is<CastableC>());

	CPPUNIT_ASSERT(!c.is<CastableA>());
	CPPUNIT_ASSERT(!c.is<CastableB>());
	CPPUNIT_ASSERT(c.is<CastableC>());
}

void CastableTest::testConstIs()
{
	const CastableA a;
	const CastableB b;
	const CastableC c;

	CPPUNIT_ASSERT(a.is<CastableA>());
	CPPUNIT_ASSERT(!a.is<CastableB>());
	CPPUNIT_ASSERT(!a.is<CastableC>());

	CPPUNIT_ASSERT(b.is<CastableA>());
	CPPUNIT_ASSERT(b.is<CastableB>());
	CPPUNIT_ASSERT(!b.is<CastableC>());

	CPPUNIT_ASSERT(!c.is<CastableA>());
	CPPUNIT_ASSERT(!c.is<CastableB>());
	CPPUNIT_ASSERT(c.is<CastableC>());
}

void CastableTest::testCast()
{
	CastableA a;
	CastableB b;
	CastableC c;

	CPPUNIT_ASSERT_NO_THROW(
		a.cast<CastableA>()
	);

	CPPUNIT_ASSERT_THROW(
		a.cast<CastableB>(),
		bad_cast
	);

	CPPUNIT_ASSERT_THROW(
		a.cast<CastableC>(),
		bad_cast
	);

	CPPUNIT_ASSERT_NO_THROW(
		b.cast<CastableA>()
	);

	CPPUNIT_ASSERT_NO_THROW(
		b.cast<CastableB>()
	);

	CPPUNIT_ASSERT_THROW(
		b.cast<CastableC>(),
		bad_cast
	);

	CPPUNIT_ASSERT_THROW(
		c.cast<CastableA>(),
		bad_cast
	);

	CPPUNIT_ASSERT_THROW(
		c.cast<CastableB>(),
		bad_cast
	);

	CPPUNIT_ASSERT_NO_THROW(
		c.cast<CastableC>()
	);
}

void CastableTest::testConstCast()
{
	const CastableA a;
	const CastableB b;
	const CastableC c;

	CPPUNIT_ASSERT_NO_THROW(
		a.cast<CastableA>()
	);

	CPPUNIT_ASSERT_THROW(
		a.cast<CastableB>(),
		bad_cast
	);

	CPPUNIT_ASSERT_THROW(
		a.cast<CastableC>(),
		bad_cast
	);

	CPPUNIT_ASSERT_NO_THROW(
		b.cast<CastableA>()
	);

	CPPUNIT_ASSERT_NO_THROW(
		b.cast<CastableB>()
	);

	CPPUNIT_ASSERT_THROW(
		b.cast<CastableC>(),
		bad_cast
	);

	CPPUNIT_ASSERT_THROW(
		c.cast<CastableA>(),
		bad_cast
	);

	CPPUNIT_ASSERT_THROW(
		c.cast<CastableB>(),
		bad_cast
	);

	CPPUNIT_ASSERT_NO_THROW(
		c.cast<CastableC>()
	);
}

}
