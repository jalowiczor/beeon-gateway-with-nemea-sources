#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>

#include "cppunit/BetterAssert.h"
#include "model/DevicePrefix.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class DevicePrefixTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(DevicePrefixTest);
	CPPUNIT_TEST(testParseSupported);
	CPPUNIT_TEST(testParseLegacy);
	CPPUNIT_TEST(testParseInvalid);
	CPPUNIT_TEST(testParseUnsupported);
	CPPUNIT_TEST(testToString);
	CPPUNIT_TEST_SUITE_END();
public:
	void testParseSupported();
	void testParseLegacy();
	void testParseInvalid();
	void testParseUnsupported();
	void testToString();

protected:
	void doTestParseSupported();
	void doTestParseLegacy();
};

CPPUNIT_TEST_SUITE_REGISTRATION(DevicePrefixTest);

void DevicePrefixTest::testParseSupported()
{
	CPPUNIT_ASSERT_NO_THROW(doTestParseSupported());
}

void DevicePrefixTest::doTestParseSupported()
{
	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_FITPROTOCOL,
		DevicePrefix::parse("fitp").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_PRESSURE_SENSOR,
		DevicePrefix::parse("psdev").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_VIRTUAL_DEVICE,
		DevicePrefix::parse("vdev").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_VPT,
		DevicePrefix::parse("vpt").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_OPENHAB,
		DevicePrefix::parse("open_hab").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_BLUETOOTH,
		DevicePrefix::parse("bluetooth").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_BELKIN_WEMO,
		DevicePrefix::parse("belkin_wemo").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_ZWAVE,
		DevicePrefix::parse("zwave").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_JABLOTRON,
		DevicePrefix::parse("jablotron").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_IQRF,
		DevicePrefix::parse("iqrf").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_LEDS,
		DevicePrefix::parse("leds").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_PHILIPS_HUE,
		DevicePrefix::parse("philips_hue").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_BLE_SMART,
		DevicePrefix::parse("ble_smart").raw());
}

void DevicePrefixTest::testParseLegacy()
{
	CPPUNIT_ASSERT_NO_THROW(doTestParseLegacy());
}

void DevicePrefixTest::doTestParseLegacy()
{
	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_FITPROTOCOL,
		DevicePrefix::parse("Fitprotocol").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_PRESSURE_SENSOR,
		DevicePrefix::parse("PressureSensor").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_PRESSURE_SENSOR,
		DevicePrefix::parse("pdev").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_VIRTUAL_DEVICE,
		DevicePrefix::parse("VirtualDevice").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_VPT,
		DevicePrefix::parse("VPT").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_OPENHAB,
		DevicePrefix::parse("OpenHAB").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_BLUETOOTH,
		DevicePrefix::parse("Bluetooth").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_BELKIN_WEMO,
		DevicePrefix::parse("BelkinWemo").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_ZWAVE,
		DevicePrefix::parse("Z-Wave").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_JABLOTRON,
		DevicePrefix::parse("Jablotron").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_IQRF,
		DevicePrefix::parse("IQRF").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_LEDS,
		DevicePrefix::parse("LEDs").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_PHILIPS_HUE,
		DevicePrefix::parse("Philips Hue").raw());

	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_BLE_SMART,
		DevicePrefix::parse("BeeWi").raw());
}

void DevicePrefixTest::testParseInvalid()
{
	CPPUNIT_ASSERT_EQUAL(
		DevicePrefix::PREFIX_INVALID,
		DevicePrefix::parse("invalid").raw());

	CPPUNIT_ASSERT_EQUAL(
		"invalid",
		DevicePrefix(DevicePrefix::PREFIX_INVALID).toString());
}

void DevicePrefixTest::testParseUnsupported()
{
	CPPUNIT_ASSERT_THROW(
		DevicePrefix::parse("nonsense"),
		InvalidArgumentException);

	CPPUNIT_ASSERT_THROW(
		DevicePrefix::parse("JAblotron"),
		InvalidArgumentException);

	CPPUNIT_ASSERT_THROW(
		DevicePrefix::parse(""),
		InvalidArgumentException);

	CPPUNIT_ASSERT_THROW(
		DevicePrefix::parse("*"),
		InvalidArgumentException);
}

void DevicePrefixTest::testToString()
{
	CPPUNIT_ASSERT_EQUAL(
		"fitp",
		DevicePrefix(DevicePrefix::PREFIX_FITPROTOCOL).toString());

	CPPUNIT_ASSERT_EQUAL(
		"psdev",
		DevicePrefix(DevicePrefix::PREFIX_PRESSURE_SENSOR).toString());

	CPPUNIT_ASSERT_EQUAL(
		"vdev",
		DevicePrefix(DevicePrefix::PREFIX_VIRTUAL_DEVICE).toString());

	CPPUNIT_ASSERT_EQUAL(
		"vpt",
		DevicePrefix(DevicePrefix::PREFIX_VPT).toString());

	CPPUNIT_ASSERT_EQUAL(
		"open_hab",
		DevicePrefix(DevicePrefix::PREFIX_OPENHAB).toString());

	CPPUNIT_ASSERT_EQUAL(
		"bluetooth",
		DevicePrefix(DevicePrefix::PREFIX_BLUETOOTH).toString());

	CPPUNIT_ASSERT_EQUAL(
		"belkin_wemo",
		DevicePrefix(DevicePrefix::PREFIX_BELKIN_WEMO).toString());

	CPPUNIT_ASSERT_EQUAL(
		"zwave",
		DevicePrefix(DevicePrefix::PREFIX_ZWAVE).toString());

	CPPUNIT_ASSERT_EQUAL(
		"jablotron",
		DevicePrefix(DevicePrefix::PREFIX_JABLOTRON).toString());

	CPPUNIT_ASSERT_EQUAL(
		"iqrf",
		DevicePrefix(DevicePrefix::PREFIX_IQRF).toString());

	CPPUNIT_ASSERT_EQUAL(
		"leds",
		DevicePrefix(DevicePrefix::PREFIX_LEDS).toString());

	CPPUNIT_ASSERT_EQUAL(
		"philips_hue",
		DevicePrefix(DevicePrefix::PREFIX_PHILIPS_HUE).toString());

	CPPUNIT_ASSERT_EQUAL(
		"ble_smart",
		DevicePrefix(DevicePrefix::PREFIX_BLE_SMART).toString());
}

}
