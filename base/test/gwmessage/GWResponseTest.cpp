#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>
#include <Poco/JSON/JSONException.h>

#include "cppunit/BetterAssert.h"
#include "gwmessage/GWResponse.h"
#include "util/JsonUtil.h"

using namespace std;
using namespace Poco;
using namespace Poco::JSON;

namespace BeeeOn {

class GWResponseTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(GWResponseTest);
	CPPUNIT_TEST(testParseStatusLegacy);
	CPPUNIT_TEST(testParseStatus);
	CPPUNIT_TEST_SUITE_END();
public:
	void testParseStatusLegacy();
	void testParseStatus();
};

CPPUNIT_TEST_SUITE_REGISTRATION(GWResponseTest);

static string jsonResponseLegacy(const GWResponse::Status &status)
{
	string json = "{"
		"\"id\": \"179b1e27-53c3-4316-999f-224a9690e253\","
		"\"message_type\": \"generic_response\","
		"\"status\": ";

	switch (status) {
	case GWResponse::ACCEPTED:
	case GWResponse::SUCCESS:
	case GWResponse::FAILED:
	case GWResponse::SUCCESS_PARTIAL:
		json += to_string(status) + "}";
		return json;
	}

	throw IllegalStateException("invalid status: " + to_string(status));
}

static string jsonResponse(const GWResponse::Status &status)
{
	string json = "{"
		"\"id\": \"179b1e27-53c3-4316-999f-224a9690e253\","
		"\"message_type\": \"generic_response\","
		"\"status\": \"";

	switch (status) {
	case GWResponse::ACCEPTED:
		json += "accepted";
		return json + "\"}";
	case GWResponse::SUCCESS:
		json += "success";
		return json + "\"}";
	case GWResponse::FAILED:
		json += "failed";
		return json + "\"}";
	case GWResponse::SUCCESS_PARTIAL:
		json += "success_partial";
		return json + "\"}";
	}

	throw IllegalStateException("invalid status: " + to_string(status));
}

void GWResponseTest::testParseStatusLegacy()
{
	const auto accepted = GWMessage::fromJSON(
		jsonResponseLegacy(GWResponse::ACCEPTED)).cast<GWResponse>();
	CPPUNIT_ASSERT_EQUAL(GWResponse::ACCEPTED, accepted->status());

	const auto success = GWMessage::fromJSON(
		jsonResponseLegacy(GWResponse::SUCCESS)).cast<GWResponse>();
	CPPUNIT_ASSERT_EQUAL(GWResponse::SUCCESS, success->status());

	const auto failed = GWMessage::fromJSON(
		jsonResponseLegacy(GWResponse::FAILED)).cast<GWResponse>();
	CPPUNIT_ASSERT_EQUAL(GWResponse::FAILED, failed->status());

	const auto successPartial = GWMessage::fromJSON(
		jsonResponseLegacy(GWResponse::SUCCESS_PARTIAL)).cast<GWResponse>();
	CPPUNIT_ASSERT_EQUAL(GWResponse::SUCCESS_PARTIAL, successPartial->status());
}

void GWResponseTest::testParseStatus()
{
	const auto accepted = GWMessage::fromJSON(
		jsonResponse(GWResponse::ACCEPTED)).cast<GWResponse>();
	CPPUNIT_ASSERT_EQUAL(GWResponse::ACCEPTED, accepted->status());

	const auto success = GWMessage::fromJSON(
		jsonResponse(GWResponse::SUCCESS)).cast<GWResponse>();
	CPPUNIT_ASSERT_EQUAL(GWResponse::SUCCESS, success->status());

	const auto failed = GWMessage::fromJSON(
		jsonResponse(GWResponse::FAILED)).cast<GWResponse>();
	CPPUNIT_ASSERT_EQUAL(GWResponse::FAILED, failed->status());

	const auto successPartial = GWMessage::fromJSON(
		jsonResponse(GWResponse::SUCCESS_PARTIAL)).cast<GWResponse>();
	CPPUNIT_ASSERT_EQUAL(GWResponse::SUCCESS_PARTIAL, successPartial->status());
}

}
