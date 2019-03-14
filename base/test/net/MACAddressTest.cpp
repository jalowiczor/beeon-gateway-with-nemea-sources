#include <set>
#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>

#include "cppunit/BetterAssert.h"
#include "net/MACAddress.h"

using namespace std;

namespace BeeeOn {

class MACAddressTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(MACAddressTest);
	CPPUNIT_TEST(testCreate);
	CPPUNIT_TEST(testCreateFromVector);
	CPPUNIT_TEST(testCreateFromArray);
	CPPUNIT_TEST(testCreateFromUint64);
	CPPUNIT_TEST(testParse);
	CPPUNIT_TEST(testOperators);
	CPPUNIT_TEST(testSeparator);
	CPPUNIT_TEST(testInto);
	CPPUNIT_TEST(testZERO);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
	void tearDown();
	void testCreate();
	void testCreateFromVector();
	void testCreateFromArray();
	void testCreateFromUint64();
	void testParse();
	void testOperators();
	void testSeparator();
	void testInto();
	void testZERO();
};

CPPUNIT_TEST_SUITE_REGISTRATION(MACAddressTest);

void MACAddressTest::setUp()
{
}

void MACAddressTest::tearDown()
{
}

void MACAddressTest::testCreate()
{
	const MACAddress mac;

	CPPUNIT_ASSERT_EQUAL(0, mac.toNumber());
	CPPUNIT_ASSERT(mac.toString() == "000000000000");
}

void MACAddressTest::testCreateFromVector()
{
	vector<unsigned char> valid = {5, 4, 3, 2, 1, 0};

	MACAddress address0(valid);
	CPPUNIT_ASSERT_EQUAL("00:01:02:03:04:05", address0.toString(':'));

	MACAddress address1({5, 4, 3, 2, 1, 0});
	CPPUNIT_ASSERT_EQUAL("00:01:02:03:04:05", address1.toString(':'));

	CPPUNIT_ASSERT_THROW(
		MACAddress address({2, 1, 0}),
		Poco::InvalidArgumentException
	);
}

void MACAddressTest::testCreateFromArray()
{
	unsigned char bytes[] = {5, 4, 3, 2, 1, 0};

	MACAddress address(bytes);
	CPPUNIT_ASSERT_EQUAL("00:01:02:03:04:05", address.toString(':'));
}

void MACAddressTest::testCreateFromUint64()
{
	const MACAddress mac(186916976722175);

	CPPUNIT_ASSERT_EQUAL(186916976722175UL, mac.toNumber());
	CPPUNIT_ASSERT(mac.toString() == "AA00000000FF");
}

void MACAddressTest::testParse()
{
	const MACAddress &mac = MACAddress::parse("AA:BB:CC:DD:EE:0");

	CPPUNIT_ASSERT_EQUAL(187723572702720UL, mac.toNumber());
	CPPUNIT_ASSERT(mac.toString() == "AABBCCDDEE00");
}

void MACAddressTest::testOperators()
{
	const MACAddress &macA = MACAddress::parse("AA:00:00:00:00:00");
	const MACAddress &macB = MACAddress::parse("AA:00:00:00:00:01");
	const MACAddress &macC = macA;

	CPPUNIT_ASSERT(macA != macB);
	CPPUNIT_ASSERT(macA == macC);
	CPPUNIT_ASSERT(macA < macB);
	CPPUNIT_ASSERT(macB > macC);
}

void MACAddressTest::testSeparator()
{
	const MACAddress &mac = MACAddress::parse("AA:0:CC:0:EE:FF");

	CPPUNIT_ASSERT(mac.toString() == "AA00CC00EEFF");
	CPPUNIT_ASSERT(mac.toString(':') == "AA:00:CC:00:EE:FF");
	CPPUNIT_ASSERT(mac.toString('-') == "AA-00-CC-00-EE-FF");
	CPPUNIT_ASSERT(mac.toString('.') == "AA.00.CC.00.EE.FF");
}

void MACAddressTest::testInto()
{
	MACAddress address(0x0000112233445566);

	unsigned char bytes[6];
	address.into(bytes);

	CPPUNIT_ASSERT_EQUAL(0x66, bytes[0]);
	CPPUNIT_ASSERT_EQUAL(0x55, bytes[1]);
	CPPUNIT_ASSERT_EQUAL(0x44, bytes[2]);
	CPPUNIT_ASSERT_EQUAL(0x33, bytes[3]);
	CPPUNIT_ASSERT_EQUAL(0x22, bytes[4]);
	CPPUNIT_ASSERT_EQUAL(0x11, bytes[5]);
}

void MACAddressTest::testZERO()
{
	unsigned char bytes[6];
	MACAddress::ZERO.into(bytes);

	CPPUNIT_ASSERT_EQUAL(0x00, bytes[0]);
	CPPUNIT_ASSERT_EQUAL(0x00, bytes[1]);
	CPPUNIT_ASSERT_EQUAL(0x00, bytes[2]);
	CPPUNIT_ASSERT_EQUAL(0x00, bytes[3]);
	CPPUNIT_ASSERT_EQUAL(0x00, bytes[4]);
	CPPUNIT_ASSERT_EQUAL(0x00, bytes[5]);
}

}
