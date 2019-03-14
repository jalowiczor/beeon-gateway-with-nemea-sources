#include <cfloat>
#include <climits>
#include <cmath>
#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Logger.h>
#include <Poco/Timespan.h>

#include "di/DIWrapper.h"
#include "cppunit/BetterAssert.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class DIWrapperTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(DIWrapperTest);
	CPPUNIT_TEST(testCreate);
	CPPUNIT_TEST(testInjectTooLongChar);
	CPPUNIT_TEST(testInjectInvalidInt);
	CPPUNIT_TEST(testInjectRefAsText);
	CPPUNIT_TEST(testPolymorphicBehaviour);
	CPPUNIT_TEST(testInheritanceOfTarget);
	CPPUNIT_TEST(testSetSharedPtr);
	CPPUNIT_TEST_SUITE_END();
public:
	void testCreate();
	void testInjectTooLongChar();
	void testInjectInvalidInt();
	void testInjectRefAsText();
	void testPolymorphicBehaviour();
	void testInheritanceOfTarget();
	void testSetSharedPtr();
};

CPPUNIT_TEST_SUITE_REGISTRATION(DIWrapperTest);

class DITest {
public:
	virtual ~DITest()
	{
	}

	void setName(const std::string &name)
	{
		m_name = name;
	}

	void setChar(char c)
	{
		m_char = c;
	}

	void setOffset(int v)
	{
		m_offset = v;
	}

	void setFactor(double f)
	{
		m_factor = f;
	}

	void setSelf(DITest *self)
	{
		m_self = self;
	}

	void setTimespan(const Timespan &time)
	{
		m_timespan = time;
	}

	void setList(const list<string> &l)
	{
		m_list.clear();

		for (auto &e : l)
			m_list.push_back(e);
	}

	void setMap(const map<string, string> &m)
	{
		m_map = m;
	}

	void call()
	{
		m_called = true;
	}

	std::string m_name;
	char m_char = '0';
	int m_offset = 0;
	double m_factor = std::nan("");
	DITest *m_self = NULL;
	Timespan m_timespan;
	vector<string> m_list;
	map<string, string> m_map;
	bool m_called = false;
};

struct DITestChild : public DITest {
};

}

BEEEON_OBJECT_BEGIN(BeeeOn, DITest)
BEEEON_OBJECT_PROPERTY("name", &DITest::setName)
BEEEON_OBJECT_PROPERTY("char", &DITest::setChar)
BEEEON_OBJECT_PROPERTY("offset", &DITest::setOffset)
BEEEON_OBJECT_PROPERTY("factor", &DITest::setFactor)
BEEEON_OBJECT_PROPERTY("self", &DITest::setSelf)
BEEEON_OBJECT_PROPERTY("timespan", &DITest::setTimespan)
BEEEON_OBJECT_PROPERTY("list", &DITest::setList)
BEEEON_OBJECT_PROPERTY("map", &DITest::setMap)
BEEEON_OBJECT_HOOK("call", &DITest::call)
BEEEON_OBJECT_END(BeeeOn, DITest)

BEEEON_OBJECT_BEGIN(BeeeOn, DITestChild)
BEEEON_OBJECT_CASTABLE(DITest)
BEEEON_OBJECT_PROPERTY("name", &DITestChild::setName)
BEEEON_OBJECT_PROPERTY("char", &DITest::setChar)
BEEEON_OBJECT_PROPERTY("offset", &DITestChild::setOffset)
BEEEON_OBJECT_PROPERTY("factor", &DITestChild::setFactor)
BEEEON_OBJECT_PROPERTY("self", &DITestChild::setSelf)
BEEEON_OBJECT_PROPERTY("timespan", &DITestChild::setTimespan)
BEEEON_OBJECT_PROPERTY("list", &DITestChild::setList)
BEEEON_OBJECT_PROPERTY("map", &DITestChild::setMap)
BEEEON_OBJECT_HOOK("call", &DITestChild::call)
BEEEON_OBJECT_END(BeeeOn, DITestChild)

namespace BeeeOn {

struct ProtectedAccess : DIWrapper {
	using DIWrapper::injectRef;
	using DIWrapper::injectNumber;
	using DIWrapper::injectText;
	using DIWrapper::injectTime;
	using DIWrapper::injectList;
	using DIWrapper::injectMap;
	using DIWrapper::callHook;
};

#define ACCESS_CALL(instance, method) \
	((instance).*&ProtectedAccess::method)

void DIWrapperTest::testCreate()
{
	DITestDIW wrapper;

	SharedPtr<DITest> test = wrapper.instance();

	CPPUNIT_ASSERT(test->m_name.empty());
	CPPUNIT_ASSERT_EQUAL('0', test->m_char);
	CPPUNIT_ASSERT_EQUAL(0, test->m_offset);
	CPPUNIT_ASSERT(std::isnan(test->m_factor));
	CPPUNIT_ASSERT(test->m_self == NULL);
	CPPUNIT_ASSERT(test->m_timespan == 0);
	CPPUNIT_ASSERT(test->m_list.empty());
	CPPUNIT_ASSERT(!test->m_called);

	ACCESS_CALL(wrapper, injectText)("name", "TEST NAME");
	ACCESS_CALL(wrapper, injectText)("char", "X");
	ACCESS_CALL(wrapper, injectNumber)("offset", 16);
	ACCESS_CALL(wrapper, injectNumber)("factor", 1.5);
	ACCESS_CALL(wrapper, injectRef)("self", wrapper);
	ACCESS_CALL(wrapper, injectTime)("timespan", 10 * Timespan::HOURS);
	ACCESS_CALL(wrapper, injectList)("list", {"a", "b", "c"});
	ACCESS_CALL(wrapper, injectMap)("map",
			{{"a", "1"}, {"b", "2"}, {"c", "3"}});
	ACCESS_CALL(wrapper, callHook)("call");

	CPPUNIT_ASSERT_EQUAL("TEST NAME", test->m_name);
	CPPUNIT_ASSERT_EQUAL('X', test->m_char);
	CPPUNIT_ASSERT_EQUAL(16, test->m_offset);
	CPPUNIT_ASSERT_EQUAL(1.5, test->m_factor);
	CPPUNIT_ASSERT(test.get() == test->m_self);
	CPPUNIT_ASSERT(Timespan(10 * Timespan::HOURS) == test->m_timespan);
	CPPUNIT_ASSERT_EQUAL("a", test->m_list[0]);
	CPPUNIT_ASSERT_EQUAL("b", test->m_list[1]);
	CPPUNIT_ASSERT_EQUAL("c", test->m_list[2]);
	CPPUNIT_ASSERT_EQUAL("1", test->m_map["a"]);
	CPPUNIT_ASSERT_EQUAL("2", test->m_map["b"]);
	CPPUNIT_ASSERT_EQUAL("3", test->m_map["c"]);
	CPPUNIT_ASSERT(test->m_called);
}

void DIWrapperTest::testInjectTooLongChar()
{
	DITestDIW wrapper;

	SharedPtr<DITest> test = wrapper.instance();

	CPPUNIT_ASSERT_EQUAL('0', test->m_char);

	CPPUNIT_ASSERT_THROW(
		ACCESS_CALL(wrapper, injectText)("char", "XX"),
		DIWWrongInputException
	);
	CPPUNIT_ASSERT_EQUAL('0', test->m_char);

	CPPUNIT_ASSERT_THROW(
		ACCESS_CALL(wrapper, injectText)("char", "long text"),
		DIWWrongInputException
	);
	CPPUNIT_ASSERT_EQUAL('0', test->m_char);
}

void DIWrapperTest::testInjectInvalidInt()
{
	DITestDIW wrapper;

	SharedPtr<DITest> test = wrapper.instance();

	CPPUNIT_ASSERT_EQUAL(0, test->m_offset);

	CPPUNIT_ASSERT_THROW(
		ACCESS_CALL(wrapper, injectNumber)("offset", 1.5),
		DIWWrongInputException
	);
	CPPUNIT_ASSERT_EQUAL(0, test->m_offset);

	CPPUNIT_ASSERT_THROW(
		ACCESS_CALL(wrapper, injectNumber)("offset", ((double) INT_MAX) + 1),
		DIWWrongInputException
	);
	CPPUNIT_ASSERT_EQUAL(0, test->m_offset);

	CPPUNIT_ASSERT_THROW(
		ACCESS_CALL(wrapper, injectNumber)("offset", ((double) INT_MIN) - 1),
		DIWWrongInputException
	);

	CPPUNIT_ASSERT_THROW(
		// 2 ^ DBL_MANT_DIG is the last value that can be exactly
		// represented by 64 bit int and thus it must always fail here
		ACCESS_CALL(wrapper, injectNumber)("offset", (((uint64_t) 1) << DBL_MANT_DIG) + 1),
		DIWWrongInputException
	);
	CPPUNIT_ASSERT_EQUAL(0, test->m_offset);
}


void DIWrapperTest::testInjectRefAsText()
{
	DITestDIW wrapper;

	SharedPtr<DITest> test = wrapper.instance();

	CPPUNIT_ASSERT(test->m_self == NULL);

	CPPUNIT_ASSERT_THROW(
		ACCESS_CALL(wrapper, injectText)("self", "test"),
		DIWWrongInputException
	);
}

void DIWrapperTest::testPolymorphicBehaviour()
{
	DIWrapper *w = new DITestDIW();
	AbstractDIWrapper<DITest> *wrapper =
		dynamic_cast<AbstractDIWrapper<DITest> *>(w);

	CPPUNIT_ASSERT(wrapper != NULL);

	SharedPtr<DITest> test = wrapper->instance();

	CPPUNIT_ASSERT(test->m_name.empty());
	CPPUNIT_ASSERT_EQUAL('0', test->m_char);
	CPPUNIT_ASSERT_EQUAL(0, test->m_offset);
	CPPUNIT_ASSERT(std::isnan(test->m_factor));
	CPPUNIT_ASSERT(test->m_self == NULL);
	CPPUNIT_ASSERT(test->m_timespan == 0);
	CPPUNIT_ASSERT(test->m_list.empty());
	CPPUNIT_ASSERT(!test->m_called);

	ACCESS_CALL(*wrapper, injectText)("name", "TEST NAME2");
	ACCESS_CALL(*wrapper, injectText)("char", "Z");
	ACCESS_CALL(*wrapper, injectNumber)("offset", 18);
	ACCESS_CALL(*wrapper, injectNumber)("factor", 1.5);
	ACCESS_CALL(*wrapper, injectRef)("self", *w);
	ACCESS_CALL(*wrapper, injectTime)("timespan", 5 * Timespan::SECONDS);
	ACCESS_CALL(*wrapper, injectList)("list", {"a", "b", "c"});
	ACCESS_CALL(*wrapper, injectMap)("map",
			{{"a", "1"}, {"b", "2"}, {"c", "3"}});
	ACCESS_CALL(*wrapper, callHook)("call");

	CPPUNIT_ASSERT_EQUAL("TEST NAME2", test->m_name);
	CPPUNIT_ASSERT_EQUAL('Z', test->m_char);
	CPPUNIT_ASSERT_EQUAL(18, test->m_offset);
	CPPUNIT_ASSERT_EQUAL(1.5, test->m_factor);
	CPPUNIT_ASSERT(test.get() == test->m_self);
	CPPUNIT_ASSERT(Timespan(5 * Timespan::SECONDS) == test->m_timespan);
	CPPUNIT_ASSERT_EQUAL("a", test->m_list[0]);
	CPPUNIT_ASSERT_EQUAL("b", test->m_list[1]);
	CPPUNIT_ASSERT_EQUAL("c", test->m_list[2]);
	CPPUNIT_ASSERT_EQUAL("1", test->m_map["a"]);
	CPPUNIT_ASSERT_EQUAL("2", test->m_map["b"]);
	CPPUNIT_ASSERT_EQUAL("3", test->m_map["c"]);
	CPPUNIT_ASSERT(test->m_called);

	delete w;
}

void DIWrapperTest::testInheritanceOfTarget()
{
	DITestChildDIW wrapper;

	SharedPtr<DITestChild> test = wrapper.instance();

	CPPUNIT_ASSERT(test->m_name.empty());
	CPPUNIT_ASSERT_EQUAL(0, test->m_offset);
	CPPUNIT_ASSERT_EQUAL('0', test->m_char);
	CPPUNIT_ASSERT(std::isnan(test->m_factor));
	CPPUNIT_ASSERT(test->m_self == NULL);
	CPPUNIT_ASSERT(test->m_timespan == 0);
	CPPUNIT_ASSERT(test->m_list.empty());
	CPPUNIT_ASSERT(!test->m_called);

	ACCESS_CALL(wrapper, injectText)("name", "TEST NAME3");
	ACCESS_CALL(wrapper, injectText)("char", "Y");
	ACCESS_CALL(wrapper, injectNumber)("offset", 19);
	ACCESS_CALL(wrapper, injectNumber)("factor", 5.2);
	ACCESS_CALL(wrapper, injectRef)("self", wrapper);
	ACCESS_CALL(wrapper, injectTime)("timespan", 16 * Timespan::DAYS);
	ACCESS_CALL(wrapper, injectList)("list", {"a", "b", "c"});
	ACCESS_CALL(wrapper, injectMap)("map",
			{{"a", "1"}, {"b", "2"}, {"c", "3"}});
	ACCESS_CALL(wrapper, callHook)("call");

	CPPUNIT_ASSERT_EQUAL("TEST NAME3", test->m_name);
	CPPUNIT_ASSERT_EQUAL('Y', test->m_char);
	CPPUNIT_ASSERT_EQUAL(19, test->m_offset);
	CPPUNIT_ASSERT_EQUAL(5.2, test->m_factor);
	CPPUNIT_ASSERT(test.get() == test->m_self);
	CPPUNIT_ASSERT(Timespan(16 * Timespan::DAYS) == test->m_timespan);
	CPPUNIT_ASSERT_EQUAL("a", test->m_list[0]);
	CPPUNIT_ASSERT_EQUAL("b", test->m_list[1]);
	CPPUNIT_ASSERT_EQUAL("c", test->m_list[2]);
	CPPUNIT_ASSERT_EQUAL("1", test->m_map["a"]);
	CPPUNIT_ASSERT_EQUAL("2", test->m_map["b"]);
	CPPUNIT_ASSERT_EQUAL("3", test->m_map["c"]);
	CPPUNIT_ASSERT(test->m_called);
}

class DIAnother {
public:
	virtual ~DIAnother()
	{
	}

	void setTest(SharedPtr<DITest> test)
	{
		m_test = test;
	}

	SharedPtr<DITest> m_test;
};

}

BEEEON_OBJECT_BEGIN(BeeeOn, DIAnother)
BEEEON_OBJECT_PROPERTY("test", &DIAnother::setTest)
BEEEON_OBJECT_END(BeeeOn, DIAnother)

namespace BeeeOn {

void DIWrapperTest::testSetSharedPtr()
{
	DIAnotherDIW wrapper;

	SharedPtr<DIAnother> another = wrapper.instance();

	CPPUNIT_ASSERT(another->m_test.isNull());

	DITestDIW test;
	ACCESS_CALL(wrapper, injectRef)("test", test);
	CPPUNIT_ASSERT_EQUAL(test.instance(), another->m_test);

	DITestChildDIW childTest;
	ACCESS_CALL(wrapper, injectRef)("test", childTest);

	DITest *rawTest = another->m_test.get();
	CPPUNIT_ASSERT(childTest.instance().get() == rawTest);
}

}
