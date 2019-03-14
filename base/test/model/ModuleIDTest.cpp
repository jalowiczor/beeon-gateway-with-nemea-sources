#include <cppunit/extensions/HelperMacros.h>

#include "cppunit/BetterAssert.h"
#include "model/ModuleID.h"

using namespace std;

namespace BeeeOn {

class ModuleIDTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ModuleIDTest);
	CPPUNIT_TEST(testCreate);
	CPPUNIT_TEST(testParse);
	CPPUNIT_TEST(testParseToString);
	CPPUNIT_TEST_SUITE_END();
public:
	void testCreate();
	void testParse();
	void testParseToString();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ModuleIDTest);

void ModuleIDTest::testCreate()
{
	const ModuleID id0(0);

	CPPUNIT_ASSERT_EQUAL(0, id0.value());
	CPPUNIT_ASSERT_EQUAL("0", id0.toString());

	const ModuleID id150(150);

	CPPUNIT_ASSERT_EQUAL(150, id150.value());
	CPPUNIT_ASSERT_EQUAL("150", id150.toString());
}

void ModuleIDTest::testParse()
{
	const ModuleID &id0 = ModuleID::parse("0");

	CPPUNIT_ASSERT_EQUAL(0, id0.value());
	CPPUNIT_ASSERT_EQUAL("0", id0.toString());

	const ModuleID &id23 = ModuleID::parse("23");

	CPPUNIT_ASSERT_EQUAL(23, id23.value());
	CPPUNIT_ASSERT_EQUAL("23", id23.toString());
}

void ModuleIDTest::testParseToString()
{
	for (int i = 0; i < 1000; ++i) {
		ModuleID id(i);
		CPPUNIT_ASSERT(id.value() == ModuleID::parse(id.toString()).value());
	}
}

}
