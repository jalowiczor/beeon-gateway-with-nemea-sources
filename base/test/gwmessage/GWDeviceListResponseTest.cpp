#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/JSONException.h>

#include "cppunit/BetterAssert.h"
#include "gwmessage/GWDeviceListResponse.h"
#include "util/JsonUtil.h"

using namespace std;
using namespace Poco;
using namespace Poco::JSON;

namespace BeeeOn {

class GWDeviceListResponseTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(GWDeviceListResponseTest);
	CPPUNIT_TEST(testCreateWithProperties);
	CPPUNIT_TEST(testParseWithProperties);
	CPPUNIT_TEST(testRefreshIsPreserved);
	CPPUNIT_TEST_SUITE_END();
public:
	void testCreateWithProperties();
	void testParseWithProperties();
	void testRefreshIsPreserved();
};

CPPUNIT_TEST_SUITE_REGISTRATION(GWDeviceListResponseTest);

static string jsonReformat(const string &json)
{
	return Dynamic::Var::toString(JsonUtil::parse(json));
}

void GWDeviceListResponseTest::testCreateWithProperties()
{
	GWDeviceListResponse::Ptr response = new GWDeviceListResponse;
	response->setProperties(
		DeviceID::parse("0xa123123412341234"),
		{
			{"ip-address", "192.168.0.1"},
			{"password", "super-secret"},
		});

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type": "device_list_response",
			"config": {
				"0xa123123412341234": {
					"ip-address": "192.168.0.1",
					"password": "super-secret"
				}
			}
		})"),
		response->toString());
}

void GWDeviceListResponseTest::testParseWithProperties()
{
	GWDeviceListResponse::Ptr response = GWMessage::fromJSON(R"({
		"message_type": "device_list_response",
		"config": {
			"0xa123123412341234": {
				"ip-address": "192.168.0.1",
				"password": "super-secret"
			}
		}
	})").cast<GWDeviceListResponse>();

	const auto properties = response->properties(DeviceID::parse("0xa123123412341234"));
	CPPUNIT_ASSERT_EQUAL("192.168.0.1", properties.at("ip-address"));
	CPPUNIT_ASSERT_EQUAL("super-secret", properties.at("password"));
}

/**
 * @brief Test that setting or clearing of properties does not remove
 * or break refresh_time in anyway.
 */
void GWDeviceListResponseTest::testRefreshIsPreserved()
{
	GWDeviceListResponse::Ptr response = new GWDeviceListResponse;
	response->setRefreshFor(
		DeviceID::parse("0xa123123412341234"),
		RefreshTime::fromSeconds(15));

	response->setProperties(
		DeviceID::parse("0xa123123412341234"),
		{
			{"ip-address", "192.168.0.1"},
			{"password", "super-secret"},
		});

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type": "device_list_response",
			"config": {
				"0xa123123412341234": {
					"refresh_time": "15",
					"ip-address": "192.168.0.1",
					"password": "super-secret"
				}
			}
		})"),
		response->toString());

	response->setProperties(
		DeviceID::parse("0xa123123412341234"),
		{});

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type": "device_list_response",
			"config": {
				"0xa123123412341234": {
					"refresh_time": "15"
				}
			}
		})"),
		response->toString());
}

}
