#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>
#include <Poco/JSON/JSONException.h>

#include "cppunit/BetterAssert.h"
#include "gwmessage/GWResponse.h"
#include "gwmessage/GWResponseWithAck.h"
#include "util/JsonUtil.h"

using namespace std;
using namespace Poco;
using namespace Poco::JSON;

namespace BeeeOn {

class GWResponseAckingTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(GWResponseAckingTest);
	CPPUNIT_TEST(testResponseAckExpected);
	CPPUNIT_TEST(testParsedResponseAckExpected);
	CPPUNIT_TEST(testResponseWithAckCompatibility);
	CPPUNIT_TEST_SUITE_END();
public:
	void testResponseAckExpected();
	void testParsedResponseAckExpected();
	void testResponseWithAckCompatibility();
};

CPPUNIT_TEST_SUITE_REGISTRATION(GWResponseAckingTest);

void GWResponseAckingTest::testResponseAckExpected()
{
	GWResponse::Ptr response = new GWResponse;

	CPPUNIT_ASSERT(!response->ackExpected());

	response->setAckExpected(true);
	CPPUNIT_ASSERT(response->ackExpected());

	CPPUNIT_ASSERT_EQUAL(
		"{\n"
		"  \"ack_expected\" : true,\n"
		"  \"message_type\" : \"generic_response\""
		"\n}",
		response->toString());
}

void GWResponseAckingTest::testParsedResponseAckExpected()
{
	GWResponse::Ptr parsed = GWMessage::fromJSON(
		"{\"message_type\": \"generic_response\", \"ack_expected\": true}")
		.cast<GWResponse>();

	CPPUNIT_ASSERT(!parsed.isNull());
	CPPUNIT_ASSERT(parsed->ackExpected());
}

void GWResponseAckingTest::testResponseWithAckCompatibility()
{
	GWResponseWithAck::Ptr withAck = new GWResponseWithAck;
	CPPUNIT_ASSERT(withAck->ackExpected());

	withAck->setAckExpected(false); // it can be disabled
	CPPUNIT_ASSERT(!withAck->ackExpected());

	GWResponseWithAck::Ptr parsed = GWMessage::fromJSON(
		"{\"message_type\": \"response_with_ack\"}")
		.cast<GWResponseWithAck>();

	CPPUNIT_ASSERT(parsed->ackExpected());
}

}
