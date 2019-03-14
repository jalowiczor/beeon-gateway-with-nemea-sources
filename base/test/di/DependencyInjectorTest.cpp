#include <Poco/AutoPtr.h>
#include <Poco/Timespan.h>
#include <Poco/Util/XMLConfiguration.h>

#include <cppunit/extensions/HelperMacros.h>

#include "di/DependencyInjector.h"
#include "cppunit/BetterAssert.h"

using namespace std;
using namespace Poco;
using namespace Poco::Util;

namespace BeeeOn {

class DependencyInjectorTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(DependencyInjectorTest);
	CPPUNIT_TEST(testSimple);
	CPPUNIT_TEST(testConstant);
	CPPUNIT_TEST(testCastToBase);
	CPPUNIT_TEST(testAlias);
	CPPUNIT_TEST(testAliasLoop);
	CPPUNIT_TEST(testAliasToAliasFails);
	CPPUNIT_TEST(testExternalVariables);
	CPPUNIT_TEST(testEarly);
	CPPUNIT_TEST(testCircularDependency);
	CPPUNIT_TEST_SUITE_END();

public:
	DependencyInjectorTest():
		m_config(new XMLConfiguration())
	{
	}

	void setUp();
	void tearDown();
	void testSimple();
	void testConstant();
	void testCastToBase();
	void testAlias();
	void testAliasLoop();
	void testAliasToAliasFails();
	void testExternalVariables();
	void testEarly();
	void testCircularDependency();

private:
	AutoPtr<XMLConfiguration> m_config;
};

CPPUNIT_TEST_SUITE_REGISTRATION(DependencyInjectorTest);

class FakeParent {
public:
	virtual ~FakeParent()
	{
	}
};

class FakeObject : public FakeParent {
public:
	FakeObject():
		m_self(NULL),
		m_index(0)
	{
	}

	virtual ~FakeObject()
	{
	}

	void setSelf(FakeObject *self)
	{
		m_self = self;
	}

	void setName(const string &name)
	{
		m_name = name;
	}

	void setOther(const string &other)
	{
		m_other = other;
	}

	void setIndex(int n)
	{
		m_index = n;
	}

	void setTimespan(const Timespan &time)
	{
		m_timespan = time;
	}

	void setList(const list<string> &l)
	{
		m_list.clear();

		for (auto &s : l)
			m_list.push_back(s);
	}

	void setMap(const map<string, string> &m)
	{
		m_map = m;
	}
public:
	FakeObject *m_self;
	string m_name;
	string m_other;
	int m_index;
	Timespan m_timespan;
	vector<string> m_list;
	map<string, string> m_map;
};

}

BEEEON_OBJECT_BEGIN(BeeeOn, FakeObject)
BEEEON_OBJECT_CASTABLE(FakeParent)
BEEEON_OBJECT_PROPERTY("self", &FakeObject::setSelf)
BEEEON_OBJECT_PROPERTY("name", &FakeObject::setName)
BEEEON_OBJECT_PROPERTY("index", &FakeObject::setIndex)
BEEEON_OBJECT_PROPERTY("other", &FakeObject::setOther)
BEEEON_OBJECT_PROPERTY("timespan", &FakeObject::setTimespan)
BEEEON_OBJECT_PROPERTY("list", &FakeObject::setList)
BEEEON_OBJECT_PROPERTY("map", &FakeObject::setMap)
BEEEON_OBJECT_END(BeeeOn, FakeObject)

namespace BeeeOn {

void DependencyInjectorTest::setUp()
{
	m_config->loadEmpty("empty");
	m_config->setString("constant[1][@name]", "label");
	m_config->setString("constant[1][@text]", "Some text");
	m_config->setString("constant[2][@name]", "sum");
	m_config->setString("constant[2][@number]", "1 + ${FakeNumber}"); // 43
	m_config->setString("constant[3][@name]", "time");
	m_config->setString("constant[3][@time]", "${sum} s"); // 43000000
	m_config->setString("constant[4][@name]", "enabled");
	m_config->setString("constant[4][@yes-when]", "${sum} == 43"); // yes
	m_config->setString("alias[1][@name]", "simpleAlias");
	m_config->setString("alias[1][@ref]", "simple");
	m_config->setString("alias[2][@name]", "secondAlias");
	m_config->setString("alias[2][@ref]", "secondAlias");
	m_config->setString("alias[3][@name]", "aliasToAlias");
	m_config->setString("alias[3][@ref]", "simpleAlias");
	m_config->setString("instance[1][@name]", "simple");
	m_config->setString("instance[1][@class]", "BeeeOn::FakeObject");
	m_config->setString("instance[1].set[1][@name]", "self");
	m_config->setString("instance[1].set[1][@ref]", "simple");
	m_config->setString("instance[1].set[2][@name]", "name");
	m_config->setString("instance[1].set[2][@text]", "fake");
	m_config->setString("instance[1].set[3][@name]", "index");
	m_config->setString("instance[1].set[3][@number]", "5");
	m_config->setString("instance[1].set[4][@name]", "list");
	m_config->setString("instance[1].set[4][@list]", "a,b,c");
	m_config->setString("instance[1].set[5][@name]", "map");
	m_config->setString("instance[1].set[5].pair[1][@key]",  "a");
	m_config->setString("instance[1].set[5].pair[1][@text]", "1");
	m_config->setString("instance[1].set[5].pair[2][@key]",  "b");
	m_config->setString("instance[1].set[5].pair[2][@text]", "2");
	m_config->setString("instance[1].set[5].pair[3][@key]",  "c");
	m_config->setString("instance[1].set[5].pair[3][@text]", "3");
	m_config->setString("instance[1].set[6][@name]", "timespan");
	m_config->setString("instance[1].set[6][@time]", "5 s");
	m_config->setString("instance[2][@name]", "variable");
	m_config->setString("instance[2][@class]", "BeeeOn::FakeObject");
	m_config->setString("instance[2].set[1][@name]", "name");
	m_config->setString("instance[2].set[1][@text]", "${FakeText}");
	m_config->setString("instance[2].set[2][@name]", "index");
	m_config->setString("instance[2].set[2][@number]", "${FakeNumber} + 6");
	m_config->setString("instance[2].set[3][@name]", "other");
	m_config->setString("instance[2].set[3][@text]", "${NotExisting}");
	m_config->setString("instance[3][@name]", "earlyInit0");
	m_config->setString("instance[3][@class]", "BeeeOn::FakeObject");
	m_config->setString("instance[3][@init]", "early");
	m_config->setString("instance[3].set[1][@name]", "name");
	m_config->setString("instance[3].set[1][@text]", "early0");
	m_config->setString("instance[4][@name]", "earlyInit1");
	m_config->setString("instance[4][@class]", "BeeeOn::FakeObject");
	m_config->setString("instance[4][@init]", "early");
	m_config->setString("instance[4].set[1][@name]", "name");
	m_config->setString("instance[4].set[1][@text]", "early1");
	m_config->setString("FakeText", "any string");
	m_config->setInt("FakeNumber", 42);

	if (Logger::get("Test").debug())
		m_config->save(cerr);
}

void DependencyInjectorTest::tearDown()
{
}

/**
 * Simple test of the most important properties of DependencyInjector.
 * It creates an instance of the FakeObject and sets ref, text and
 * number on it.
 */
void DependencyInjectorTest::testSimple()
{
	DependencyInjector injector(m_config);

	SharedPtr<FakeObject> fake = injector.find<FakeObject>("simple");
	CPPUNIT_ASSERT(fake.isNull());

	fake = injector.create<FakeObject>("simple");

	CPPUNIT_ASSERT(!fake.isNull());
	CPPUNIT_ASSERT(fake->m_self == fake);
	CPPUNIT_ASSERT(fake->m_name.compare("fake") == 0);
	CPPUNIT_ASSERT(fake->m_index == 5);
	CPPUNIT_ASSERT(Timespan(5 * Timespan::SECONDS) == fake->m_timespan);
	CPPUNIT_ASSERT_EQUAL(3, fake->m_list.size());
	CPPUNIT_ASSERT_EQUAL("a", fake->m_list[0]);
	CPPUNIT_ASSERT_EQUAL("b", fake->m_list[1]);
	CPPUNIT_ASSERT_EQUAL("c", fake->m_list[2]);
	CPPUNIT_ASSERT_EQUAL("1", fake->m_map["a"]);
	CPPUNIT_ASSERT_EQUAL("2", fake->m_map["b"]);
	CPPUNIT_ASSERT_EQUAL("3", fake->m_map["c"]);
}

void DependencyInjectorTest::testConstant()
{
	DependencyInjector injector(m_config);

	CPPUNIT_ASSERT(m_config->has("label"));
	CPPUNIT_ASSERT_EQUAL("Some text", m_config->getString("label"));
	CPPUNIT_ASSERT(m_config->has("sum"));
	CPPUNIT_ASSERT_EQUAL("43", m_config->getString("sum"));
	CPPUNIT_ASSERT(m_config->has("time"));
	CPPUNIT_ASSERT_EQUAL("43000000", m_config->getString("time"));
	CPPUNIT_ASSERT(m_config->has("enabled"));
	CPPUNIT_ASSERT_EQUAL("yes", m_config->getString("enabled"));
}

void DependencyInjectorTest::testCastToBase()
{
	DependencyInjector injector(m_config);

	SharedPtr<FakeParent> parent;

	parent = injector.find<FakeParent>("simple");
	CPPUNIT_ASSERT(parent.isNull());

	parent = injector.create<FakeParent>("simple");
	CPPUNIT_ASSERT(!parent.isNull());

	SharedPtr<FakeObject> object = parent.cast<FakeObject>();
	CPPUNIT_ASSERT(!object.isNull());
}

/**
 * Test refering to an alias pointing to an instance.
 * An alias points to an instance so it just uses a different name.
 */
void DependencyInjectorTest::testAlias()
{
	DependencyInjector injector(m_config);

	SharedPtr<FakeObject> fakeAlias = injector.find<FakeObject>("simpleAlias");
	CPPUNIT_ASSERT(fakeAlias.isNull());

	fakeAlias = injector.create<FakeObject>("simpleAlias");
	CPPUNIT_ASSERT(!fakeAlias.isNull());

	SharedPtr<FakeObject> fake = injector.find<FakeObject>("simple");
	CPPUNIT_ASSERT(!fake.isNull());

	SharedPtr<FakeObject> fakeCreated = injector.create<FakeObject>("simple");
	CPPUNIT_ASSERT(!fakeCreated.isNull());

	CPPUNIT_ASSERT(fake == fakeAlias);
	CPPUNIT_ASSERT(fakeAlias == fakeCreated);
}

/**
 * Test alias points to itself throws an exception.
 */
void DependencyInjectorTest::testAliasLoop()
{
	DependencyInjector injector(m_config);

	CPPUNIT_ASSERT_THROW(injector.create<FakeObject>("secondAlias"),
			IllegalStateException);
}

/**
 * Alias cannot point to an alias.
 * The NotFoundException is thrown because the DependencyInjector
 * does not search the alias namespace recursively.
 */
void DependencyInjectorTest::testAliasToAliasFails()
{
	DependencyInjector injector(m_config);

	CPPUNIT_ASSERT_THROW(injector.create<FakeObject>("aliasToAlias"),
			NotFoundException);
}

/**
 * Test that ${VAR} constructs are expanded when constructing objects.
 */
void DependencyInjectorTest::testExternalVariables()
{
	DependencyInjector injector(m_config);

	SharedPtr<FakeObject> fake = injector.create<FakeObject>("variable");

	CPPUNIT_ASSERT(!fake.isNull());
	CPPUNIT_ASSERT(fake->m_name.compare("any string") == 0);
	CPPUNIT_ASSERT(fake->m_other.compare("${NotExisting}") == 0);
	CPPUNIT_ASSERT(fake->m_index == (42 + 6));
}

/**
 * Test whether the init="early" instances are created automatically
 * during DependencyInjector construction.
 */
void DependencyInjectorTest::testEarly()
{
	DependencyInjector injector(m_config);

	SharedPtr<FakeObject> early0 = injector.find<FakeObject>("earlyInit0");
	CPPUNIT_ASSERT(!early0.isNull());

	SharedPtr<FakeObject> early1 = injector.find<FakeObject>("earlyInit1");
	CPPUNIT_ASSERT(!early1.isNull());

	CPPUNIT_ASSERT(early0 != early1);
}

class CircularDependencyObject {
public:
	CircularDependencyObject():
		m_other(NULL),
		m_destroyed(NULL)
	{
	}

	virtual ~CircularDependencyObject()
	{
		if (m_destroyed)
			*m_destroyed = true;
	}

	void setOther(SharedPtr<CircularDependencyObject> other)
	{
		m_other = other;
	}

	void cleanup()
	{
		m_other = NULL;
	}

	SharedPtr<CircularDependencyObject> m_other;
	bool *m_destroyed;
};

}

BEEEON_OBJECT_BEGIN(BeeeOn, CircularDependencyObject)
BEEEON_OBJECT_PROPERTY("other", &CircularDependencyObject::setOther)
BEEEON_OBJECT_HOOK("cleanup", &CircularDependencyObject::cleanup)
BEEEON_OBJECT_END(BeeeOn, CircularDependencyObject)

namespace BeeeOn {

/**
 * Show that two objects that holds a shared pointer to each other
 * blocks each other's destruction. The solution is to avoid using
 * shared pointer or to define the cleanup hook. The cleanup hook
 * is used in this test to make sure that the objects do not block
 * each other from destruction.
 */
void DependencyInjectorTest::testCircularDependency()
{
	AutoPtr<XMLConfiguration> m_config(new XMLConfiguration);

	m_config->loadEmpty("empty");
	m_config->setString("instance[1][@name]", "first");
	m_config->setString("instance[1][@class]", "BeeeOn::CircularDependencyObject");
	m_config->setString("instance[1].set[1][@name]", "other");
	m_config->setString("instance[1].set[1][@ref]", "second");
	m_config->setString("instance[2][@name]", "second");
	m_config->setString("instance[2][@class]", "BeeeOn::CircularDependencyObject");
	m_config->setString("instance[2].set[1][@name]", "other");
	m_config->setString("instance[2].set[1][@ref]", "first");

	SharedPtr<CircularDependencyObject> first;
	SharedPtr<CircularDependencyObject> second;

	bool firstDestroyed = false;
	bool secondDestroyed = false;

	{
		DependencyInjector injector(m_config);

		first = injector.create<CircularDependencyObject>("first");
		// 3 references: DependencyInjector, second, and this method
		CPPUNIT_ASSERT_EQUAL(3, first.referenceCount());

		second = injector.create<CircularDependencyObject>("second");
		// 3 references: DependencyInjector, first, and this method
		CPPUNIT_ASSERT_EQUAL(3, second.referenceCount());

		first->m_destroyed = &firstDestroyed;
		second->m_destroyed = &secondDestroyed;
	}

	CPPUNIT_ASSERT(!firstDestroyed);
	CPPUNIT_ASSERT(!secondDestroyed);

	// DependencyInjector has dropped references
	// "cleanup" hook dropped the circular ones
	CPPUNIT_ASSERT_EQUAL(1, first.referenceCount());
	CPPUNIT_ASSERT_EQUAL(1, second.referenceCount());

	first = NULL;
	CPPUNIT_ASSERT(firstDestroyed);

	second = NULL;
	CPPUNIT_ASSERT(secondDestroyed);
}

}
