#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>

#include "cppunit/BetterAssert.h"
#include "util/MultiException.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class MultiExceptionTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(MultiExceptionTest);
	CPPUNIT_TEST(testCommonCase);
	CPPUNIT_TEST(testSaneAfterThrow);
	CPPUNIT_TEST(testOutOfMemory);
	CPPUNIT_TEST_SUITE_END();
public:
	void testCommonCase();
	void testSaneAfterThrow();
	void testOutOfMemory();
};

CPPUNIT_TEST_SUITE_REGISTRATION(MultiExceptionTest);

/**
 * Test common scenario of using the MultiException. Just collect many
 * exceptions in a loop and check the resulted message.
 */
void MultiExceptionTest::testCommonCase()
{
	MultiException multi;

	for (int i = 0; i < 5; ++i) {
		try {
			throw LogicException("XXX");
		}
		catch (const Exception &e) {
			multi.caught(e);
		}
	}

	CPPUNIT_ASSERT(!multi.empty());
	size_t count = 0;

	for (const auto &e : multi) {
		CPPUNIT_ASSERT_EQUAL("XXX", e->message());
		count += 1;
	}

	CPPUNIT_ASSERT_EQUAL(5, count);

	CPPUNIT_ASSERT_EQUAL(
		"caught multiple exceptions: \n"
		"Logic exception: XXX\n"
		"Logic exception: XXX\n"
		"Logic exception: XXX\n"
		"...and more",
		multi.message());

	CPPUNIT_ASSERT_EQUAL(
		"MultiException: caught multiple exceptions: \n"
		"Logic exception: XXX\n"
		"Logic exception: XXX\n"
		"Logic exception: XXX\n"
		"...and more",
		multi.displayText());
}

void MultiExceptionTest::testSaneAfterThrow()
{
	MultiException multi;

	for (int i = 0; i < 5; ++i) {
		try {
			throw LogicException("XXX");
		}
		catch (const Exception &e) {
			multi.caught(e);
		}
	}

	CPPUNIT_ASSERT(!multi.empty());

	try {
		multi.rethrow();
	}
	catch (const MultiException &e) {
		CPPUNIT_ASSERT(!e.empty());

		CPPUNIT_ASSERT_EQUAL(
			"caught multiple exceptions: \n"
			"Logic exception: XXX\n"
			"Logic exception: XXX\n"
			"Logic exception: XXX\n"
			"...and more",
			e.message());

		CPPUNIT_ASSERT_EQUAL(
			"MultiException: caught multiple exceptions: \n"
			"Logic exception: XXX\n"
			"Logic exception: XXX\n"
			"Logic exception: XXX\n"
			"...and more",
			e.displayText());
	}
}

class TestingThrowingE : public Exception {
public:
	TestingThrowingE():
		Exception("failing")
	{
	}

	Exception *clone() const override
	{
		throw bad_alloc();
	}
};

/**
 * Test that when Exception::clone() fails due to bad_alloc, we store
 * the message and use a builtin OutOfMemoryException instance
 * instead of the given one.
 */
void MultiExceptionTest::testOutOfMemory()
{
	MultiException multi;
	LogicException e0("X");
	TestingThrowingE e1;
	LogicException e2("Y");

	multi.caught(e0);
	multi.caught(e1);
	multi.caught(e2);

	CPPUNIT_ASSERT(!multi.empty());
	CPPUNIT_ASSERT_EQUAL(
		"caught multiple exceptions: \n"
		"Logic exception: X\n"
		"Exception: failing\n"
		"Logic exception: Y",
		multi.message());

	auto it = multi.begin();
	CPPUNIT_ASSERT_EQUAL(e0.className(), (*it)->className());
	++it;
	CPPUNIT_ASSERT_EQUAL(typeid(OutOfMemoryException).name(), (*it)->className());
	++it;
	CPPUNIT_ASSERT_EQUAL(e2.className(), (*it)->className());
}

}
