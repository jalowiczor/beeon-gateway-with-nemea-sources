#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/JSONException.h>

#include "cppunit/BetterAssert.h"
#include "gwmessage/GWDeviceAcceptRequest.h"
#include "util/JsonUtil.h"

using namespace std;
using namespace Poco;
using namespace Poco::JSON;

namespace BeeeOn {

class GWDeviceAcceptRequestTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(GWDeviceAcceptRequestTest);
	CPPUNIT_TEST(testCreateWithProperties);
	CPPUNIT_TEST(testParseWithProperties);
	CPPUNIT_TEST_SUITE_END();
public:
	void testCreateWithProperties();
	void testParseWithProperties();
};

CPPUNIT_TEST_SUITE_REGISTRATION(GWDeviceAcceptRequestTest);

static string jsonReformat(const string &json)
{
	return Dynamic::Var::toString(JsonUtil::parse(json));
}

void GWDeviceAcceptRequestTest::testCreateWithProperties()
{
	GWDeviceAcceptRequest::Ptr response = new GWDeviceAcceptRequest;
	response->setProperties({
		{"ip-address", "192.168.0.1"},
		{"password", "super-secret"},
	});

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type": "device_accept_request",
			"config": {
				"ip-address": "192.168.0.1",
				"password": "super-secret"
			}
		})"),
		response->toString());
}

void GWDeviceAcceptRequestTest::testParseWithProperties()
{
	GWDeviceAcceptRequest::Ptr response = GWMessage::fromJSON(R"({
		"message_type": "device_accept_request",
		"config": {
			"ip-address": "192.168.0.1",
			"password": "super-secret"
		}
	})").cast<GWDeviceAcceptRequest>();

	const auto properties = response->properties();
	CPPUNIT_ASSERT_EQUAL("192.168.0.1", properties.at("ip-address"));
	CPPUNIT_ASSERT_EQUAL("super-secret", properties.at("password"));
}

}
