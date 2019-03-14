#include <cppunit/extensions/HelperMacros.h>

#include "cppunit/BetterAssert.h"
#include "model/DeviceDescription.h"

#include <iostream>

using namespace Poco;
using namespace std;

namespace BeeeOn {

class DeviceDescriptionTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(DeviceDescriptionTest);
	CPPUNIT_TEST(testCreate);
	CPPUNIT_TEST(testProductNameAndVendor);
	CPPUNIT_TEST(testRefreshTime);
	CPPUNIT_TEST_SUITE_END();
public:
	void testCreate();
	void testProductNameAndVendor();
	void testRefreshTime();
};

CPPUNIT_TEST_SUITE_REGISTRATION(DeviceDescriptionTest);

void DeviceDescriptionTest::testCreate()
{
	set<ModuleType::Attribute> attributes;
	attributes.emplace(ModuleType::Attribute::ATTR_INNER);
	ModuleType type(ModuleType::Type::fromRaw(
		ModuleType::Type::TYPE_HUMIDITY), attributes);

	list<ModuleType> types;
	types.push_back(type);

	DeviceDescription description = DeviceDescription::Builder()
		.id(DeviceID::parse("0xfe01020304050607"))
		.type("Good Company", "Nice Product")
		.modules(types)
		.refreshTime(Timespan(30, 0))
		.build();

	CPPUNIT_ASSERT_EQUAL("0xfe01020304050607", description.id().toString());
	CPPUNIT_ASSERT_EQUAL("Good Company", description.vendor());
	CPPUNIT_ASSERT_EQUAL("Nice Product", description.productName());
	CPPUNIT_ASSERT(description.refreshTime() == RefreshTime::fromSeconds(30));

	const list<ModuleType> &checkTypes = description.dataTypes();
	CPPUNIT_ASSERT_EQUAL("humidity", checkTypes.begin()->type().toString());
	CPPUNIT_ASSERT_EQUAL("inner", checkTypes.begin()->attributes().begin()->toString());
}

/**
 * Tests the replacement of invalid characters in the product name and vendor.
 * The replacement is needed due to security and the sending of names in JSON.
 */
void DeviceDescriptionTest::testProductNameAndVendor()
{
	DeviceDescription description1 = DeviceDescription::Builder()
		.id(DeviceID::parse("0xfe01020304050607"))
		.type("Company© #1®", "Product #1®")
		.build();

	CPPUNIT_ASSERT_EQUAL("Company© #1®", description1.vendor());
	CPPUNIT_ASSERT_EQUAL("Product #1®", description1.productName());

	DeviceDescription description2 = DeviceDescription::Builder()
		.id(DeviceID::parse("0xfe01020304050607"))
		.type("Skvělá továrna",	"Čistá žárovka")
		.build();

	CPPUNIT_ASSERT_EQUAL("Skvělá továrna", description2.vendor());
	CPPUNIT_ASSERT_EQUAL("Čistá žárovka", description2.productName());

	string vendor = "Good Company";
	vendor[4] = 0;
	string name = "Nice Product";
	name[4] = 0;
	DeviceDescription description3 = DeviceDescription::Builder()
		.id(DeviceID::parse("0xfe01020304050607"))
		.type(vendor, name)
		.build();

	CPPUNIT_ASSERT_EQUAL("Good?Company", description3.vendor());
	CPPUNIT_ASSERT_EQUAL("Nice?Product", description3.productName());

	DeviceDescription description4 = DeviceDescription::Builder()
		.id(DeviceID::parse("0xfe01020304050607"))
		.type("Milan`s Company", "Milan`s Macbook Pro")
		.build();

	CPPUNIT_ASSERT_EQUAL("Milan?s Company", description4.vendor());
	CPPUNIT_ASSERT_EQUAL("Milan?s Macbook Pro", description4.productName());

	DeviceDescription description5 = DeviceDescription::Builder()
		.id(DeviceID::parse("0xfe01020304050607"))
		.type("&%@~;\\\"*|¬§¶¼\n\t\b\r", "&%@~;\\\"*|¬§¶¼\n\t\b\r")
		.build();

	CPPUNIT_ASSERT_EQUAL("?????????????????", description5.vendor());
	CPPUNIT_ASSERT_EQUAL("?????????????????", description5.productName());

	DeviceDescription description6 = DeviceDescription::Builder()
		.id(DeviceID::parse("0xfe01020304050607"))
		.type("\x41\xc1\x81\xe0\x81\x81", "\x41\xc1\x81\xe0\x81\x81")
		.build();

	CPPUNIT_ASSERT_EQUAL("A??", description6.vendor());
	CPPUNIT_ASSERT_EQUAL("A??", description6.productName());
}

/**
 * Tests the normalization of refresh time.
 */
void DeviceDescriptionTest::testRefreshTime()
{
	DeviceDescription description1 = DeviceDescription::Builder()
		.id(DeviceID::parse("0xfe01020304050607"))
		.type("Good Company", "Nice Product")
		.disabledRefreshTime()
		.build();

	CPPUNIT_ASSERT(Timespan(0) == description1.refreshTime());

	DeviceDescription description2 = DeviceDescription::Builder()
		.id(DeviceID::parse("0xfe01020304050607"))
		.type("Good Company", "Nice Product")
		.refreshTime(5 * Timespan::SECONDS)
		.build();

	CPPUNIT_ASSERT(Timespan(5 * Timespan::SECONDS) == description2.refreshTime());

	DeviceDescription description3 = DeviceDescription::Builder()
		.id(DeviceID::parse("0xfe01020304050607"))
		.type("Good Company", "Nice Product")
		.refreshTime(50 * Timespan::MILLISECONDS)
		.build();

	CPPUNIT_ASSERT(Timespan(1 * Timespan::SECONDS) == description3.refreshTime());

	DeviceDescription description4 = DeviceDescription::Builder()
		.id(DeviceID::parse("0xfe01020304050607"))
		.type("Good Company", "Nice Product")
		.noRefreshTime()
		.build();

	CPPUNIT_ASSERT(Timespan(-1) == description4.refreshTime());
}

}
