#include <algorithm>

#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>
#include <Poco/JSON/JSONException.h>

#include "cppunit/BetterAssert.h"
#include "gwmessage/GWMessage.h"
#include "gwmessage/GWGatewayRegister.h"
#include "gwmessage/GWGatewayAccepted.h"
#include "gwmessage/GWRequest.h"
#include "gwmessage/GWResponse.h"
#include "gwmessage/GWAck.h"
#include "gwmessage/GWResponseWithAck.h"
#include "gwmessage/GWSensorDataConfirm.h"
#include "gwmessage/GWSensorDataExport.h"
#include "gwmessage/GWNewDeviceGroupRequest.h"
#include "gwmessage/GWNewDeviceRequest.h"
#include "gwmessage/GWNoticeRequest.h"
#include "gwmessage/GWLastValueResponse.h"
#include "gwmessage/GWLastValueRequest.h"
#include "gwmessage/GWDeviceListResponse.h"
#include "gwmessage/GWDeviceListRequest.h"
#include "gwmessage/GWListenRequest.h"
#include "gwmessage/GWUnpairRequest.h"
#include "gwmessage/GWSearchRequest.h"
#include "gwmessage/GWSetValueRequest.h"
#include "gwmessage/GWDeviceAcceptRequest.h"
#include "model/GlobalID.h"
#include "model/DeviceDescription.h"
#include "util/JsonUtil.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;
using namespace Poco::JSON;

namespace BeeeOn {

class GWMessageTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(GWMessageTest);
	CPPUNIT_TEST(testParseEmpty);
	CPPUNIT_TEST(testParseEmptyObject);
	CPPUNIT_TEST(testParseUnknownType);
	CPPUNIT_TEST(testParseGatewayRegister);
	CPPUNIT_TEST(testCreateGatewayRegister);
	CPPUNIT_TEST(testParseGatewayAccepted);
	CPPUNIT_TEST(testCreateGatewayAccepted);
	CPPUNIT_TEST(testDeriveResponse);
	CPPUNIT_TEST(testGetAckFromResponse);
	CPPUNIT_TEST(testParseSensorDataConfirm);
	CPPUNIT_TEST(testCreateSensorDataConfirm);
	CPPUNIT_TEST(testParseSensorDataExport);
	CPPUNIT_TEST(testCreateSensorDataExport);
	CPPUNIT_TEST(testGetConfirmFromSensorDataExport);
	CPPUNIT_TEST(testParseNewDeviceLegacy);
	CPPUNIT_TEST(testParseNewDevice);
	CPPUNIT_TEST(testCreateNewDevice);
	CPPUNIT_TEST(testParseNewDeviceGroup);
	CPPUNIT_TEST(testCreateNewDeviceGroup);
	CPPUNIT_TEST(testParseNotice);
	CPPUNIT_TEST(testCreateNotice);
	CPPUNIT_TEST(testParseLastValue);
	CPPUNIT_TEST(testCreateLastValue);
	CPPUNIT_TEST(testParseDeviceList);
	CPPUNIT_TEST(testParseDeviceListWithValues);
	CPPUNIT_TEST(testCreateDeviceList);
	CPPUNIT_TEST(testCreateDeviceListWithValues);
	CPPUNIT_TEST(testParseListen);
	CPPUNIT_TEST(testCreateListen);
	CPPUNIT_TEST(testParseSearchIP);
	CPPUNIT_TEST(testParseSearchMAC);
	CPPUNIT_TEST(testParseSearchSerial);
	CPPUNIT_TEST(testCreateSearchIP);
	CPPUNIT_TEST(testCreateSearchMAC);
	CPPUNIT_TEST(testCreateSearchSerial);
	CPPUNIT_TEST(testParseUnpair);
	CPPUNIT_TEST(testCreateUnpair);
	CPPUNIT_TEST(testParseSetValueLegacy);
	CPPUNIT_TEST(testParseSetValue);
	CPPUNIT_TEST(testCreateSetValueLegacy);
	CPPUNIT_TEST(testCreateSetValue);
	CPPUNIT_TEST(testParseDeviceAccept);
	CPPUNIT_TEST(testCreateDeviceAccept);
	CPPUNIT_TEST_SUITE_END();
public:
	void testParseEmpty();
	void testParseEmptyObject();
	void testParseUnknownType();
	void testParseGatewayRegister();
	void testCreateGatewayRegister();
	void testParseGatewayAccepted();
	void testCreateGatewayAccepted();
	void testDeriveResponse();
	void testGetAckFromResponse();
	void testParseSensorDataConfirm();
	void testCreateSensorDataConfirm();
	void testParseSensorDataExport();
	void testCreateSensorDataExport();
	void testGetConfirmFromSensorDataExport();
	void testParseNewDeviceLegacy();
	void testParseNewDevice();
	void testCreateNewDevice();
	void testParseNewDeviceGroup();
	void testCreateNewDeviceGroup();
	void testParseNotice();
	void testCreateNotice();
	void testParseLastValue();
	void testCreateLastValue();
	void testParseDeviceList();
	void testParseDeviceListWithValues();
	void testCreateDeviceList();
	void testCreateDeviceListWithValues();
	void testParseListen();
	void testCreateListen();
	void testParseSearchIP();
	void testParseSearchMAC();
	void testParseSearchSerial();
	void testCreateSearch();
	void testCreateSearchIP();
	void testCreateSearchMAC();
	void testCreateSearchSerial();
	void testParseUnpair();
	void testCreateUnpair();
	void testParseSetValueLegacy();
	void testParseSetValue();
	void testCreateSetValueLegacy();
	void testCreateSetValue();
	void testParseDeviceAccept();
	void testCreateDeviceAccept();
protected:
	string jsonReformat(const string &json);
};

CPPUNIT_TEST_SUITE_REGISTRATION(GWMessageTest);

string GWMessageTest::jsonReformat(const string &json)
{
	return Dynamic::Var::toString(JsonUtil::parse(json));
}

void GWMessageTest::testParseEmpty()
{
	CPPUNIT_ASSERT_THROW(
		GWMessage::fromJSON(""),
		JSONException
	);
}

void GWMessageTest::testParseEmptyObject()
{
	CPPUNIT_ASSERT_THROW(
		GWMessage::fromJSON("{}"),
		InvalidAccessException
	);
}

void GWMessageTest::testParseUnknownType()
{
	CPPUNIT_ASSERT_THROW(
		GWMessage::fromJSON(R"({"message_type" : "unknown_type"})"),
		InvalidArgumentException
	);
}

void GWMessageTest::testParseGatewayRegister()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
			"message_type" : "gateway_register",
			"gateway_id" : "1863705252509532",
			"version" : "v1.0",
			"ip_address" : "192.168.1.1"
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::GATEWAY_REGISTER, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWGatewayRegister>().isNull());

	GWGatewayRegister::Ptr registerMessage = message.cast<GWGatewayRegister>();
	CPPUNIT_ASSERT(registerMessage->gatewayID().toString() == "1863705252509532");
	CPPUNIT_ASSERT(registerMessage->version() == "v1.0");
	CPPUNIT_ASSERT(registerMessage->ipAddress().toString() == "192.168.1.1");
}

void GWMessageTest::testCreateGatewayRegister()
{
	GWGatewayRegister::Ptr message(new GWGatewayRegister);
	message->setGatewayID(GatewayID::parse("1863705252509532"));
	message->setVersion("v1.0");
	message->setIPAddress(Net::IPAddress("192.168.1.1"));

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type" : "gateway_register",
			"gateway_id" : "1863705252509532",
			"version" : "v1.0",
			"ip_address" : "192.168.1.1"
		})"),
		message->toString()
	);
}

void GWMessageTest::testParseGatewayAccepted()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
			"message_type" : "gateway_accepted"
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::GATEWAY_ACCEPTED, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWGatewayAccepted>().isNull());
}

void GWMessageTest::testCreateGatewayAccepted()
{
	GWGatewayAccepted::Ptr message(new GWGatewayAccepted);

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type" : "gateway_accepted"
		})"),
		message->toString()
	);
}

class TestingResponse : public GWResponse {
public:
	typedef SharedPtr<TestingResponse> Ptr;

	void setText(const string &text)
	{
		json()->set("text", text);
	}

	string text() const
	{
		return json()->getValue<string>("text");
	}
};

class TestingRequest : public GWRequest {
public:
	TestingRequest():
		GWRequest(GWMessageType::GATEWAY_REGISTER)
	{
	}

	GWResponse::Ptr deriveResponse() const override
	{
		return deriveGenericResponse(new TestingResponse);
	}
};

void GWMessageTest::testDeriveResponse()
{
	TestingRequest request;
	request.setID(GlobalID::parse("4a41d041-eb1e-4e9c-9528-1bbe74f54d59"));

	GWResponse::Ptr resp1 = request.derive();
	CPPUNIT_ASSERT(GWMessageType::GENERIC_RESPONSE == resp1->type());
	CPPUNIT_ASSERT_EQUAL("4a41d041-eb1e-4e9c-9528-1bbe74f54d59", resp1->id().toString());

	TestingResponse::Ptr resp2 = request.derive<TestingResponse>(
		[](TestingResponse::Ptr r) {
			r->setText("test text");
		}
	);

	CPPUNIT_ASSERT(GWMessageType::GENERIC_RESPONSE == resp1->type());
	CPPUNIT_ASSERT_EQUAL("test text", resp2->text());
	CPPUNIT_ASSERT_EQUAL("4a41d041-eb1e-4e9c-9528-1bbe74f54d59", resp2->id().toString());
}

void GWMessageTest::testGetAckFromResponse()
{
	GWResponseWithAck::Ptr resp(new GWResponseWithAck);
	resp->setID(GlobalID::parse("4a41d041-eb1e-4e9c-9528-1bbe74f54d59"));
	resp->setStatus(GWResponse::SUCCESS);

	GWAck::Ptr ack = resp->ack();

	CPPUNIT_ASSERT(!ack.isNull());
	CPPUNIT_ASSERT_EQUAL(GWMessageType::GENERIC_ACK, ack->type().raw());
	CPPUNIT_ASSERT_EQUAL("4a41d041-eb1e-4e9c-9528-1bbe74f54d59", ack->id().toString());
	CPPUNIT_ASSERT_EQUAL(GWResponse::SUCCESS, ack->status());
}

void GWMessageTest::testParseSensorDataConfirm()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
			"message_type" : "sensor_data_confirm",
			"id" : "4a41d041-eb1e-4e9c-9528-1bbe74f54d59"
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::SENSOR_DATA_CONFIRM, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWSensorDataConfirm>().isNull());

	GWSensorDataConfirm::Ptr confirm = message.cast<GWSensorDataConfirm>();
	CPPUNIT_ASSERT_EQUAL("4a41d041-eb1e-4e9c-9528-1bbe74f54d59", confirm->id().toString() );
}

void GWMessageTest::testCreateSensorDataConfirm()
{
	GWSensorDataConfirm::Ptr message(new GWSensorDataConfirm);
	message->setID(GlobalID::parse("4a41d041-eb1e-4e9c-9528-1bbe74f54d59"));

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type" : "sensor_data_confirm",
			"id" : "4a41d041-eb1e-4e9c-9528-1bbe74f54d59"
		})"),
		message->toString()
	);
}

void GWMessageTest::testParseSensorDataExport()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
			"message_type" : "sensor_data_export",
			"id" : "4a41d041-eb1e-4e9c-9528-1bbe74f54d59",
			"data" : [
				{
					"device_id" : "0xa123123412341234",
					"timestamp" : 1500334250150150,
					"values": [
						{
							"module_id" : "0",
							"valid" : true,
							"value" : 3.5
						}
					]
				},
				{
					"device_id" : "0xa123444455556666",
					"timestamp" : 1111111111111111,
					"values": [
						{
							"module_id" : "0",
							"valid" : true,
							"value" : 3.5
						},
						{
							"module_id": "1",
							"valid": false
						}
					]
				}
			]
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::SENSOR_DATA_EXPORT, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWSensorDataExport>().isNull());

	GWSensorDataExport::Ptr dataExport = message.cast<GWSensorDataExport>();
	CPPUNIT_ASSERT_EQUAL("4a41d041-eb1e-4e9c-9528-1bbe74f54d59", dataExport->id().toString() );

	vector<SensorData> data = dataExport->data();
	CPPUNIT_ASSERT(data.size() == 2);

	CPPUNIT_ASSERT_EQUAL("0xa123123412341234", data[0].deviceID().toString());
	CPPUNIT_ASSERT_EQUAL(1500334250150150, data[0].timestamp().value().epochMicroseconds());
	CPPUNIT_ASSERT_EQUAL(0, data[0].begin()->moduleID().value());
	CPPUNIT_ASSERT_EQUAL(3.5, data[0].begin()->value());
	CPPUNIT_ASSERT(data[0].begin()->isValid());

	CPPUNIT_ASSERT_EQUAL("0xa123444455556666", data[1].deviceID().toString());
	CPPUNIT_ASSERT_EQUAL(1111111111111111, data[1].timestamp().value().epochMicroseconds());
	CPPUNIT_ASSERT_EQUAL(0, data[1].begin()->moduleID().value());
	CPPUNIT_ASSERT_EQUAL(3.5, data[1].begin()->value());
	CPPUNIT_ASSERT(data[1].begin()->isValid());
	CPPUNIT_ASSERT_EQUAL(1, (data[1].begin() + 1)->moduleID().value());
	CPPUNIT_ASSERT(!(data[1].begin() + 1)->isValid());
}

void GWMessageTest::testCreateSensorDataExport()
{
	GWSensorDataExport::Ptr message(new GWSensorDataExport);
	message->setID(GlobalID::parse("4a41d041-eb1e-4e9c-9528-1bbe74f54d59"));

	SensorValue value1;
	value1.setModuleID(0);
	value1.setValue(3.5);
	value1.setValid(true);

	SensorValue value2;
	value2.setModuleID(1);
	value2.setValue(NAN);
	value2.setValid(false);

	SensorData data1(
		DeviceID::parse("0xa123123412341234"),
		Timestamp(1500334250150150),
		{value1});

	SensorData data2(
		DeviceID::parse("0xa123444455556666"),
		Timestamp(1111111111111111),
		{value1, value2});

	vector<SensorData> dataVector;
	dataVector.push_back(data1);
	dataVector.push_back(data2);

	message->setData(dataVector);

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type" : "sensor_data_export",
			"id" : "4a41d041-eb1e-4e9c-9528-1bbe74f54d59",
			"data" : [
				{
					"device_id" : "0xa123123412341234",
					"timestamp" : 1500334250150150,
					"values": [
						{
							"module_id" : "0",
							"valid" : true,
							"value" : 3.5
						}
					]
				},
				{
					"device_id" : "0xa123444455556666",
					"timestamp" : 1111111111111111,
					"values": [
						{
							"module_id" : "0",
							"valid" : true,
							"value" : 3.5
						},
						{
							"module_id": "1",
							"valid": false
						}
					]
				}
			]
		})"),
		message->toString()
	);
}

void GWMessageTest::testGetConfirmFromSensorDataExport()
{
	GWSensorDataExport::Ptr message(new GWSensorDataExport);
	message->setID(GlobalID::parse("4a41d041-eb1e-4e9c-9528-1bbe74f54d59"));

	GWSensorDataConfirm::Ptr confirm = message->confirm();

	CPPUNIT_ASSERT(!confirm.isNull());
	CPPUNIT_ASSERT_EQUAL(GWMessageType::SENSOR_DATA_CONFIRM, confirm->type().raw());
	CPPUNIT_ASSERT_EQUAL("4a41d041-eb1e-4e9c-9528-1bbe74f54d59", confirm->id().toString());
}

void GWMessageTest::testParseNewDeviceLegacy()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
			"message_type" : "new_device_request",
			"id" : "4a41d041-eb1e-4e9c-9528-1bbe74f54d59",
			"device_id" : "0xfe01020304050607",
			"vendor" : "Good Company",
			"product_name" : "Nice Product",
			"refresh_time" : 30,
			"module_types" : [
				{
					"type" : "humidity",
					"attributes" : [
						{"attribute" : "inner"}
					]
				},
				{
					"type" : "pressure",
					"attributes" : [
						{"attribute" : "outer"},
						{"attribute" : "manual-only"},
						{"attribute" : "controllable"}
					]
				},
				{
					"type" : "enum",
					"subtype": "SOME_ID",
					"attributes": []
				}
			]
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::NEW_DEVICE_REQUEST, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWNewDeviceRequest>().isNull());

	GWNewDeviceRequest::Ptr request = message.cast<GWNewDeviceRequest>();
	CPPUNIT_ASSERT_EQUAL("4a41d041-eb1e-4e9c-9528-1bbe74f54d59", request->id().toString());
	CPPUNIT_ASSERT_EQUAL("0xfe01020304050607", request->deviceID().toString());
	CPPUNIT_ASSERT_EQUAL("Good Company", request->vendor());
	CPPUNIT_ASSERT_EQUAL("Nice Product", request->productName());
	CPPUNIT_ASSERT(Timespan(30, 0) == request->refreshTime());

	const list<ModuleType> &types = request->moduleTypes();
	auto type = types.begin();

	CPPUNIT_ASSERT(type != types.end());
	CPPUNIT_ASSERT_EQUAL("humidity", type->type().toString());
	CPPUNIT_ASSERT_EQUAL(1, type->attributes().size());
	CPPUNIT_ASSERT_EQUAL("inner", type->attributes().begin()->toString());

	++type;
	CPPUNIT_ASSERT(type != types.end());
	CPPUNIT_ASSERT_EQUAL("pressure", type->type().toString());
	CPPUNIT_ASSERT_EQUAL(3, type->attributes().size());
	CPPUNIT_ASSERT_EQUAL("manual-only", type->attributes().begin()->toString());

	++type;
	CPPUNIT_ASSERT(type != types.end());
	CPPUNIT_ASSERT_EQUAL("enum", type->type().toString());
	CPPUNIT_ASSERT_EQUAL("SOME_ID", type->customTypeID().toString());
	CPPUNIT_ASSERT(type->attributes().empty());

	++type;
	CPPUNIT_ASSERT(type == types.end());
}

void GWMessageTest::testParseNewDevice()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
			"message_type" : "new_device_request",
			"id" : "4a41d041-eb1e-4e9c-9528-1bbe74f54d59",
			"device_id" : "0xfe01020304050607",
			"vendor" : "Good Company",
			"product_name" : "Nice Product",
			"refresh_time" : 30,
			"module_types" : [
				{
					"type" : "humidity",
					"attributes" : ["inner"]
				},
				{
					"type" : "pressure",
					"attributes" : ["outer", "manual-only", "controllable"]
				},
				{
					"type" : "enum",
					"subtype": "SOME_ID",
					"attributes": []
				}
			]
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::NEW_DEVICE_REQUEST, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWNewDeviceRequest>().isNull());

	GWNewDeviceRequest::Ptr request = message.cast<GWNewDeviceRequest>();
	CPPUNIT_ASSERT_EQUAL("4a41d041-eb1e-4e9c-9528-1bbe74f54d59", request->id().toString());
	CPPUNIT_ASSERT_EQUAL("0xfe01020304050607", request->deviceID().toString());
	CPPUNIT_ASSERT_EQUAL("Good Company", request->vendor());
	CPPUNIT_ASSERT_EQUAL("Nice Product", request->productName());
	CPPUNIT_ASSERT(Timespan(30, 0) == request->refreshTime());

	const list<ModuleType> &types = request->moduleTypes();
	auto type = types.begin();

	CPPUNIT_ASSERT(type != types.end());
	CPPUNIT_ASSERT_EQUAL("humidity", type->type().toString());
	CPPUNIT_ASSERT_EQUAL(1, type->attributes().size());
	CPPUNIT_ASSERT_EQUAL("inner", type->attributes().begin()->toString());

	++type;
	CPPUNIT_ASSERT(type != types.end());
	CPPUNIT_ASSERT_EQUAL("pressure", type->type().toString());
	CPPUNIT_ASSERT_EQUAL(3, type->attributes().size());
	CPPUNIT_ASSERT_EQUAL("manual-only", type->attributes().begin()->toString());

	++type;
	CPPUNIT_ASSERT(type != types.end());
	CPPUNIT_ASSERT_EQUAL("enum", type->type().toString());
	CPPUNIT_ASSERT_EQUAL("SOME_ID", type->customTypeID().toString());
	CPPUNIT_ASSERT(type->attributes().empty());

	++type;
	CPPUNIT_ASSERT(type == types.end());
}

void GWMessageTest::testCreateNewDevice()
{
	GWNewDeviceRequest::Ptr request(new GWNewDeviceRequest);
	request->setID(GlobalID::parse("4a41d041-eb1e-4e9c-9528-1bbe74f54d59"));
	request->setDeviceID(DeviceID::parse("0xfe01020304050607"));
	request->setVendor("Good Company");
	request->setProductName("Nice Product");
	request->setRefreshTime(Timespan(30, 0));

	set<ModuleType::Attribute> attributes1;
	attributes1.emplace(ModuleType::Attribute::ATTR_INNER);
	ModuleType type1(ModuleType::Type::fromRaw(
		ModuleType::Type::TYPE_HUMIDITY), attributes1);

	set<ModuleType::Attribute> attributes2;
	attributes2.emplace(ModuleType::Attribute::ATTR_MANUAL_ONLY);
	attributes2.emplace(ModuleType::Attribute::ATTR_OUTER);
	attributes2.emplace(ModuleType::Attribute::ATTR_CONTROLLABLE);
	ModuleType type2(ModuleType::Type::fromRaw(
		ModuleType::Type::TYPE_PRESSURE), attributes2);

	list<ModuleType> types;
	types.push_back(type1);
	types.push_back(type2);

	request->setModuleTypes(types);

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type" : "new_device_request",
			"id" : "4a41d041-eb1e-4e9c-9528-1bbe74f54d59",
			"device_id" : "0xfe01020304050607",
			"vendor" : "Good Company",
			"product_name" : "Nice Product",
			"refresh_time" : 30,
			"module_types" : [
				{
					"type" : "humidity",
					"attributes" : [
						{"attribute" : "inner"}
					]
				},
				{
					"type" : "pressure",
					"attributes" : [
						{"attribute" : "manual-only"},
						{"attribute" : "outer"},
						{"attribute" : "controllable"}
					]
				}
			]
		})"),
		request->toString()
	);
}

void GWMessageTest::testParseNewDeviceGroup()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
			"message_type" : "new_device_group_request",
			"id" : "4a41d041-eb1e-4e9c-9528-1bbe74f54d59",
			"group_name" : "Perfect Two",
			"vendor" : "Good Company",
			"devices" : [
				{
					"device_id" : "0xfe01020304050607",
					"product_name" : "Nice Product",
					"refresh_time" : 30,
					"module_types" : [
						{
							"type" : "humidity",
							"attributes" : [
								"inner"
							]
						}
					]
				},
				{
					"device_id" : "0xfe01020304050608",
					"product_name" : "Very Nice Product",
					"refresh_time" : 30,
					"module_types" : [
						{
							"type" : "temperature",
							"attributes" : [
								"inner"
							]
						}
					]
				}
			]
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::NEW_DEVICE_GROUP_REQUEST, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWNewDeviceGroupRequest>().isNull());

	GWNewDeviceGroupRequest::Ptr request = message.cast<GWNewDeviceGroupRequest>();
	CPPUNIT_ASSERT_EQUAL("4a41d041-eb1e-4e9c-9528-1bbe74f54d59", request->id().toString());
	CPPUNIT_ASSERT_EQUAL("Perfect Two", request->groupName());
	CPPUNIT_ASSERT_EQUAL("Good Company", request->vendor());

	vector<DeviceDescription> descriptions = request->deviceDescriptions();
	auto it = descriptions.begin();
	CPPUNIT_ASSERT_EQUAL("0xfe01020304050607", it->id().toString());
	CPPUNIT_ASSERT_EQUAL("Good Company", it->vendor());
	CPPUNIT_ASSERT_EQUAL("Nice Product", it->productName());
	CPPUNIT_ASSERT(Timespan(30, 0) == it->refreshTime());

	const list<ModuleType> &types1 = it->dataTypes();
	CPPUNIT_ASSERT_EQUAL("humidity", types1.begin()->type().toString());
	CPPUNIT_ASSERT_EQUAL("inner", types1.begin()->attributes().begin()->toString());

	it++;
	CPPUNIT_ASSERT_EQUAL("0xfe01020304050608", it->id().toString());
	CPPUNIT_ASSERT_EQUAL("Good Company", it->vendor());
	CPPUNIT_ASSERT_EQUAL("Very Nice Product", it->productName());
	CPPUNIT_ASSERT(Timespan(30, 0) == it->refreshTime());

	const list<ModuleType> &types2 = it->dataTypes();
	CPPUNIT_ASSERT_EQUAL("temperature", types2.begin()->type().toString());
	CPPUNIT_ASSERT_EQUAL("inner", types2.begin()->attributes().begin()->toString());
}

void GWMessageTest::testCreateNewDeviceGroup()
{
	GWNewDeviceGroupRequest::Ptr request(new GWNewDeviceGroupRequest);
	request->setID(GlobalID::parse("4a41d041-eb1e-4e9c-9528-1bbe74f54d59"));
	request->setGroupName("Perfect Two");
	request->setVendor("Good Company");

	set<ModuleType::Attribute> attributes1;
	attributes1.emplace(ModuleType::Attribute::ATTR_INNER);
	ModuleType type1(ModuleType::Type::fromRaw(
		ModuleType::Type::TYPE_HUMIDITY), attributes1);

	list<ModuleType> types1;
	types1.push_back(type1);

	DeviceDescription description1 = DeviceDescription::Builder()
		.id(DeviceID::parse("0xfe01020304050607"))
		.type("Good Company", "Nice Product")
		.modules(types1)
		.refreshTime(Timespan(30, 0))
		.build();

	set<ModuleType::Attribute> attributes2;
	attributes2.emplace(ModuleType::Attribute::ATTR_INNER);
	ModuleType type2(ModuleType::Type::fromRaw(
		ModuleType::Type::TYPE_TEMPERATURE), attributes2);

	list<ModuleType> types2;
	types2.push_back(type2);

	DeviceDescription description2 = DeviceDescription::Builder()
		.id(DeviceID::parse("0xfe01020304050608"))
		.type("Good Company", "Very Nice Product")
		.modules(types2)
		.refreshTime(Timespan(30, 0))
		.build();

	request->addDeviceDescription(description1);
	request->addDeviceDescription(description2);

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type" : "new_device_group_request",
			"id" : "4a41d041-eb1e-4e9c-9528-1bbe74f54d59",
			"group_name" : "Perfect Two",
			"vendor" : "Good Company",
			"devices" : [
				{
					"device_id" : "0xfe01020304050607",
					"product_name" : "Nice Product",
					"refresh_time" : 30,
					"module_types" : [
						{
							"type" : "humidity",
							"attributes" : [
								{"attribute" : "inner"}
							]
						}
					]
				},
				{
					"device_id" : "0xfe01020304050608",
					"product_name" : "Very Nice Product",
					"refresh_time" : 30,
					"module_types" : [
						{
							"type" : "temperature",
							"attributes" : [
								{"attribute" : "inner"}
							]
						}
					]
				}
			]
		})"),
		request->toString()
	);
}

void GWMessageTest::testParseNotice()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
		"message_type": "notice_request",
		"id": "dd451efb-a806-4ddf-ae77-d0df4490b96d",
		"at": 1546274217435091,
		"severity": "warn",
		"key": "missing.dongle",
		"context": {
			"gateway_id": "1213558016461165",
			"technology": "zwave"
		}
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::NOTICE_REQUEST, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWNoticeRequest>().isNull());

	GWNoticeRequest::Ptr notice = message.cast<GWNoticeRequest>();
	CPPUNIT_ASSERT_EQUAL("dd451efb-a806-4ddf-ae77-d0df4490b96d", notice->id().toString());
	CPPUNIT_ASSERT_EQUAL(1546274217435091, notice->at().epochMicroseconds());
	CPPUNIT_ASSERT_EQUAL("warn", notice->severity().toString());
	CPPUNIT_ASSERT_EQUAL("missing.dongle", notice->key());
	CPPUNIT_ASSERT(!notice->context().isNull());
	CPPUNIT_ASSERT_EQUAL("1213558016461165", notice->context()->getValue<string>("gateway_id"));
	CPPUNIT_ASSERT_EQUAL("zwave", notice->context()->getValue<string>("technology"));
}

void GWMessageTest::testCreateNotice()
{
	GWNoticeRequest::Ptr notice = new GWNoticeRequest;
	const Timestamp at(1546274217435091);

	notice->setID(GlobalID::parse("495b7a34-d2e7-4cc7-afcc-0690fa5f072a"));
	notice->setAt(at);
	notice->setSeverity(Severity::INFO);
	notice->setKey("device.dead");

	Object::Ptr context = new Object;
	context->set("device_id", "0xa801020311223344");
	notice->setContext(context);

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"at": 1546274217435091,
			"context": {
				"device_id": "0xa801020311223344"
			},
			"id": "495b7a34-d2e7-4cc7-afcc-0690fa5f072a",
			"key": "device.dead",
			"message_type": "notice_request",
			"severity": "info"
		})"),
		notice->toString()
	);
}

void GWMessageTest::testParseLastValue()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
			"message_type": "last_value_response",
			"id": "495b7a34-d2e7-4cc7-afcc-0690fa5f072a",
			"status": 1,
			"value": 78.5,
			"valid": true
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::LAST_VALUE_RESPONSE, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWLastValueResponse>().isNull());

	GWLastValueResponse::Ptr resp = message.cast<GWLastValueResponse>();
	CPPUNIT_ASSERT_EQUAL("495b7a34-d2e7-4cc7-afcc-0690fa5f072a", resp->id().toString());
	CPPUNIT_ASSERT_EQUAL(GWResponse::Status::SUCCESS, resp->status());
	CPPUNIT_ASSERT_EQUAL(78.5, resp->value());
	CPPUNIT_ASSERT(resp->valid());

	message = GWMessage::fromJSON(
	R"({
			"message_type": "last_value_request",
			"id": "495b7a34-d2e7-4cc7-afcc-0690fa5f072a",
			"device_id": "0xfe01020304050607",
			"module_id": "0"
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::LAST_VALUE_REQUEST, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWLastValueRequest>().isNull());

	GWLastValueRequest::Ptr request = message.cast<GWLastValueRequest>();
	CPPUNIT_ASSERT_EQUAL("495b7a34-d2e7-4cc7-afcc-0690fa5f072a", request->id().toString());
	CPPUNIT_ASSERT_EQUAL("0xfe01020304050607", request->deviceID().toString());
	CPPUNIT_ASSERT_EQUAL("0", request->moduleID().toString());
}

void GWMessageTest::testCreateLastValue()
{
	GWLastValueResponse::Ptr response(new GWLastValueResponse);
	response->setID(GlobalID::parse("a08c356b-316d-4690-84d4-b77d95b403fe"));
	response->setStatus(GWResponse::Status::SUCCESS);
	response->setValue(80);
	response->setValid(true);

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type": "last_value_response",
			"id": "a08c356b-316d-4690-84d4-b77d95b403fe",
			"status": 1,
			"value": 80,
			"valid": true
		})"),
		response->toString()
	);

	GWLastValueRequest::Ptr request(new GWLastValueRequest);
	request->setID(GlobalID::parse("a08c356b-316d-4690-84d4-b77d95b403fe"));
	request->setDeviceID(DeviceID::parse("0xfe01020304050607"));
	request->setModuleID(0);

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type": "last_value_request",
			"id": "a08c356b-316d-4690-84d4-b77d95b403fe",
			"device_id": "0xfe01020304050607",
			"module_id": "0"
		})"),
		request->toString()
	);
}

void GWMessageTest::testParseDeviceList()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
			"message_type": "device_list_response",
			"id": "495b7a34-d2e7-4cc7-afcc-0690fa5f072a",
			"status": 1,
			"devices": [
				{"device_id": "0xa15410132465788"},
				{"device_id": "0xa15410132465789"}
			],
			"values" : {
			    "0xa15410132465788" : {
			      "0" : 0,
			      "1" : 45.5
			    },
			    "0xa15410132465789" : {
			      "0" : -54.2
			    }
			},
			"config" : {
			    "0xa15410132465789" : {
			      "refresh_time" : "30"
			    }
			}
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::DEVICE_LIST_RESPONSE, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWDeviceListResponse>().isNull());

	GWDeviceListResponse::Ptr response = message.cast<GWDeviceListResponse>();
	CPPUNIT_ASSERT_EQUAL("495b7a34-d2e7-4cc7-afcc-0690fa5f072a", response->id().toString());
	CPPUNIT_ASSERT_EQUAL(GWResponse::Status::SUCCESS, response->status());
	CPPUNIT_ASSERT(response->refreshFor(0xa15410132465788UL).isNone());
	CPPUNIT_ASSERT_EQUAL(30, response->refreshFor(0xa15410132465789UL).seconds());

	vector<DeviceID> devices = response->devices();
	CPPUNIT_ASSERT_EQUAL("0xa15410132465788", devices[0].toString());
	CPPUNIT_ASSERT_EQUAL("0xa15410132465789", devices[1].toString());

	const auto values0 = response->modulesValues(DeviceID::parse("0xa15410132465788"));
	CPPUNIT_ASSERT_EQUAL(2, values0.size());
	CPPUNIT_ASSERT_EQUAL(0.0, values0.at(0));
	CPPUNIT_ASSERT_EQUAL(45.5, values0.at(1));

	const auto values1 = response->modulesValues(DeviceID::parse("0xa15410132465789"));
	CPPUNIT_ASSERT_EQUAL(1, values1.size());
	CPPUNIT_ASSERT_EQUAL(-54.2, values1.at(0));

	message = GWMessage::fromJSON(
	R"({
			"message_type": "device_list_request",
			"id": "495b7a34-d2e7-4cc7-afcc-0690fa5f072a",
			"device_prefix": "Fitprotocol"
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::DEVICE_LIST_REQUEST, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWDeviceListRequest>().isNull());

	GWDeviceListRequest::Ptr request = message.cast<GWDeviceListRequest>();
	CPPUNIT_ASSERT_EQUAL("495b7a34-d2e7-4cc7-afcc-0690fa5f072a", request->id().toString());
	CPPUNIT_ASSERT_EQUAL("fitp", request->devicePrefix().toString());
}

void GWMessageTest::testParseDeviceListWithValues()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
			"message_type": "device_list_response",
			"id": "495b7a34-d2e7-4cc7-afcc-0690fa5f072a",
			"status": 1,
			"devices": [
				{"device_id": "0xa15410132465788"},
				{"device_id": "0xa15410132465789"},
				{"device_id": "0xa15410132465790"}
			],
			"values": {
				"0xa15410132465788": {
					"0": 15.0,
					"2": 24.4
				},
				"0xa15410132465790": {
					"1": -1.0
				}
			}
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::DEVICE_LIST_RESPONSE, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWDeviceListResponse>().isNull());

	GWDeviceListResponse::Ptr response = message.cast<GWDeviceListResponse>();
	CPPUNIT_ASSERT_EQUAL("495b7a34-d2e7-4cc7-afcc-0690fa5f072a", response->id().toString());
	CPPUNIT_ASSERT_EQUAL(GWResponse::Status::SUCCESS, response->status());

	vector<DeviceID> devices = response->devices();
	CPPUNIT_ASSERT_EQUAL("0xa15410132465788", devices[0].toString());
	CPPUNIT_ASSERT_EQUAL("0xa15410132465789", devices[1].toString());
	CPPUNIT_ASSERT_EQUAL("0xa15410132465790", devices[2].toString());

	const auto &values0 = response->modulesValues(0xa15410132465788);
	CPPUNIT_ASSERT_EQUAL(2, values0.size());
	CPPUNIT_ASSERT_EQUAL(15.0, values0.at(0));
	CPPUNIT_ASSERT_EQUAL(24.4, values0.at(2));

	const auto &values1 = response->modulesValues(0xa15410132465789);
	CPPUNIT_ASSERT(values1.empty());

	const auto &values2 = response->modulesValues(0xa15410132465790);
	CPPUNIT_ASSERT_EQUAL(1, values2.size());
	CPPUNIT_ASSERT_EQUAL(-1.0, values2.at(1));
}

void GWMessageTest::testCreateDeviceList()
{
	GWDeviceListResponse::Ptr response(new GWDeviceListResponse);
	response->setID(GlobalID::parse("a08c356b-316d-4690-84d4-b77d95b403fe"));
	response->setStatus(GWResponse::Status::SUCCESS);

	vector<DeviceID> devices;
	devices.push_back(DeviceID::parse("0xa1001234567890a0"));
	devices.push_back(DeviceID::parse("0xa1001234567890a1"));
	response->setDevices(devices);

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type": "device_list_response",
			"id": "a08c356b-316d-4690-84d4-b77d95b403fe",
			"status": 1,
			"devices": [
				{"device_id": "0xa1001234567890a0"},
				{"device_id": "0xa1001234567890a1"}
			]
		})"),
		response->toString()
	);

	GWDeviceListRequest::Ptr request(new GWDeviceListRequest);
	request->setID(GlobalID::parse("a08c356b-316d-4690-84d4-b77d95b403fe"));
	request->setDevicePrefix(DevicePrefix::parse("Fitprotocol"));

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type": "device_list_request",
			"id": "a08c356b-316d-4690-84d4-b77d95b403fe",
			"device_prefix": "fitp"
		})"),
		request->toString()
	);
}

void GWMessageTest::testCreateDeviceListWithValues()
{
	GWDeviceListResponse::Ptr response(new GWDeviceListResponse);
	response->setID(GlobalID::parse("a08c356b-316d-4690-84d4-b77d95b403fe"));
	response->setStatus(GWResponse::Status::SUCCESS);
	response->setRefreshFor(0xa1001234567890a0UL, RefreshTime::fromSeconds(60));

	vector<DeviceID> devices;
	devices.push_back(DeviceID::parse("0xa1001234567890a0"));
	devices.push_back(DeviceID::parse("0xa1001234567890a1"));
	response->setDevices(devices);
	response->setModulesValues(
		DeviceID::parse("0xa1001234567890a0"),
		{{0, 20.0}, {3, 1.0}});
	response->setModulesValues(
		DeviceID::parse("0xa1001234567890a1"),
		{{2, 0.0}});

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type": "device_list_response",
			"id": "a08c356b-316d-4690-84d4-b77d95b403fe",
			"config": {
				"0xa1001234567890a0": {
					"refresh_time": "60"
				}
			},
			"status": 1,
			"devices": [
				{"device_id": "0xa1001234567890a0"},
				{"device_id": "0xa1001234567890a1"}
			],
			"values": {
				"0xa1001234567890a0": {
					"0": 20.0,
					"3": 1.0
				},
				"0xa1001234567890a1": {
					"2": 0.0
				}
			}
		})"),
		response->toString()
	);
}

void GWMessageTest::testParseListen()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
			"message_type" : "listen_request",
			"duration" : 30
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::LISTEN_REQUEST, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWListenRequest>().isNull());

	GWListenRequest::Ptr request = message.cast<GWListenRequest>();
	CPPUNIT_ASSERT(request->duration() == Timespan(30, 0));
}

void GWMessageTest::testCreateListen()
{
	GWListenRequest::Ptr request(new GWListenRequest);
	request->setID(GlobalID::parse("4a41d041-eb1e-4e9c-9528-1bbe74f54d59"));
	request->setDuration(Timespan(60, 0));

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type": "listen_request",
			"id": "4a41d041-eb1e-4e9c-9528-1bbe74f54d59",
			"duration": 60
		})"),
		request->toString()
	);
}

void GWMessageTest::testParseSearchIP()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
		"message_type" : "search_request",
		"duration" : 20,
		"criteria": {
			"ip_address": "192.168.0.1"
		}
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::SEARCH_REQUEST, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWSearchRequest>().isNull());

	GWSearchRequest::Ptr request = message.cast<GWSearchRequest>();
	CPPUNIT_ASSERT(request->duration() == Timespan(20, 0));
	CPPUNIT_ASSERT(request->criteria().hasIPAddress());
	CPPUNIT_ASSERT(!request->criteria().hasMACAddress());
	CPPUNIT_ASSERT(!request->criteria().hasSerialNumber());

	CPPUNIT_ASSERT_EQUAL("192.168.0.1", request->criteria().ipAddress().toString());
}

void GWMessageTest::testParseSearchMAC()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
		"message_type" : "search_request",
		"duration" : 20,
		"criteria": {
			"mac_address": "ab:01:bc:02:cd:03"
		}
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::SEARCH_REQUEST, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWSearchRequest>().isNull());

	GWSearchRequest::Ptr request = message.cast<GWSearchRequest>();
	CPPUNIT_ASSERT(request->duration() == Timespan(20, 0));
	CPPUNIT_ASSERT(!request->criteria().hasIPAddress());
	CPPUNIT_ASSERT(request->criteria().hasMACAddress());
	CPPUNIT_ASSERT(!request->criteria().hasSerialNumber());

	CPPUNIT_ASSERT_EQUAL("AB:01:BC:02:CD:03", request->criteria().macAddress().toString(':'));
}

void GWMessageTest::testParseSearchSerial()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
		"message_type" : "search_request",
		"duration" : 20,
		"criteria": {
			"serial_number": "1234567890"
		}
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::SEARCH_REQUEST, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWSearchRequest>().isNull());

	GWSearchRequest::Ptr request = message.cast<GWSearchRequest>();
	CPPUNIT_ASSERT(request->duration() == Timespan(20, 0));
	CPPUNIT_ASSERT(!request->criteria().hasIPAddress());
	CPPUNIT_ASSERT(!request->criteria().hasMACAddress());
	CPPUNIT_ASSERT(request->criteria().hasSerialNumber());

	CPPUNIT_ASSERT_EQUAL(1234567890UL, request->criteria().serialNumber());
}

void GWMessageTest::testCreateSearchIP()
{
	GWSearchRequest::Ptr request = new GWSearchRequest;
	request->setID(GlobalID::parse("5a705ebf-aff8-4649-8b81-449f619127bd"));
	request->setDuration(Timespan(60, 0));
	request->setCriteria({IPAddress("10.0.0.1")});

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type": "search_request",
			"id": "5a705ebf-aff8-4649-8b81-449f619127bd",
			"duration": 60,
			"criteria": {
				"ip_address": "10.0.0.1"
			}
		})"),
		request->toString()
	);
}

void GWMessageTest::testCreateSearchMAC()
{
	GWSearchRequest::Ptr request = new GWSearchRequest;
	request->setID(GlobalID::parse("5a705ebf-aff8-4649-8b81-449f619127bd"));
	request->setDuration(Timespan(60, 0));
	request->setCriteria({MACAddress::parse("01:02:03:04:05:06")});

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type": "search_request",
			"id": "5a705ebf-aff8-4649-8b81-449f619127bd",
			"duration": 60,
			"criteria": {
				"mac_address": "01:02:03:04:05:06"
			}
		})"),
		request->toString()
	);
}

void GWMessageTest::testCreateSearchSerial()
{
	GWSearchRequest::Ptr request = new GWSearchRequest;
	request->setID(GlobalID::parse("5a705ebf-aff8-4649-8b81-449f619127bd"));
	request->setDuration(Timespan(60, 0));
	request->setCriteria({1234567890UL});

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type": "search_request",
			"id": "5a705ebf-aff8-4649-8b81-449f619127bd",
			"duration": 60,
			"criteria": {
				"serial_number": "1234567890"
			}
		})"),
		request->toString()
	);
}

void GWMessageTest::testParseUnpair()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
			"message_type": "unpair_request",
			"id": "4a41d041-eb1e-4e9c-9528-1bbe74f54d59",
			"device_id": "0xfe01020304050607"
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::UNPAIR_REQUEST, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWUnpairRequest>().isNull());

	GWUnpairRequest::Ptr request = message.cast<GWUnpairRequest>();
	CPPUNIT_ASSERT_EQUAL("4a41d041-eb1e-4e9c-9528-1bbe74f54d59", request->id().toString());
	CPPUNIT_ASSERT_EQUAL("0xfe01020304050607", request->deviceID().toString());
}

void GWMessageTest::testCreateUnpair()
{
	GWUnpairRequest::Ptr request(new GWUnpairRequest);
	request->setID(GlobalID::parse("4a41d041-eb1e-4e9c-9528-1bbe74f54d59"));
	request->setDeviceID(DeviceID::parse("0xfe01020304050607"));

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type": "unpair_request",
			"id": "4a41d041-eb1e-4e9c-9528-1bbe74f54d59",
			"device_id": "0xfe01020304050607"
		})"),
		request->toString()
	);
}

void GWMessageTest::testParseSetValueLegacy()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
			"message_type" : "set_value_request",
			"id" : "4a41d041-eb1e-4e9c-9528-1bbe74f54d59",
			"device_id" : "0xfe01020304050607",
			"module_id" : 0,
			"value" : 3.5,
			"timeout" : 10
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::SET_VALUE_REQUEST, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWSetValueRequest>().isNull());

	GWSetValueRequest::Ptr request = message.cast<GWSetValueRequest>();
	CPPUNIT_ASSERT_EQUAL("4a41d041-eb1e-4e9c-9528-1bbe74f54d59", request->id().toString());
	CPPUNIT_ASSERT_EQUAL("0xfe01020304050607", request->deviceID().toString());
	CPPUNIT_ASSERT_EQUAL("0", request->moduleID().toString());
	CPPUNIT_ASSERT_EQUAL(OpMode::TRY_ONCE, request->mode());
	CPPUNIT_ASSERT(3.5);
	CPPUNIT_ASSERT(request->timeout() == Timespan(10, 0));
}

void GWMessageTest::testParseSetValue()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
			"message_type" : "set_value_request",
			"id" : "4a41d041-eb1e-4e9c-9528-1bbe74f54d59",
			"device_id" : "0xfe01020304050607",
			"module_id" : 0,
			"value" : 3.5,
			"timeout" : 10,
			"mode": "try_harder"
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::SET_VALUE_REQUEST, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWSetValueRequest>().isNull());

	GWSetValueRequest::Ptr request = message.cast<GWSetValueRequest>();
	CPPUNIT_ASSERT_EQUAL("4a41d041-eb1e-4e9c-9528-1bbe74f54d59", request->id().toString());
	CPPUNIT_ASSERT_EQUAL("0xfe01020304050607", request->deviceID().toString());
	CPPUNIT_ASSERT_EQUAL("0", request->moduleID().toString());
	CPPUNIT_ASSERT_EQUAL(OpMode::TRY_HARDER, request->mode());
	CPPUNIT_ASSERT(3.5);
	CPPUNIT_ASSERT(request->timeout() == Timespan(10, 0));
}

void GWMessageTest::testCreateSetValueLegacy()
{
	GWSetValueRequest::Ptr request1(new GWSetValueRequest);
	request1->setID(GlobalID::parse("4a41d041-eb1e-4e9c-9528-1bbe74f54d59"));
	request1->setDeviceID(DeviceID::parse("0xfe01020304050607"));
	request1->setModuleID(0);
	request1->setValue(3.5);
	request1->setTimeout(Timespan(10, 0));

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type" : "set_value_request",
			"id" : "4a41d041-eb1e-4e9c-9528-1bbe74f54d59",
			"device_id" : "0xfe01020304050607",
			"module_id" : 0,
			"value" : 3.5,
			"timeout" : 10
		})"),
		request1->toString()
	);

	GWSetValueRequest::Ptr request2(new GWSetValueRequest);
	CPPUNIT_ASSERT_THROW(request2->setValue(NAN), InvalidArgumentException);
}


void GWMessageTest::testCreateSetValue()
{
	GWSetValueRequest::Ptr request1(new GWSetValueRequest);
	request1->setID(GlobalID::parse("4a41d041-eb1e-4e9c-9528-1bbe74f54d59"));
	request1->setDeviceID(DeviceID::parse("0xfe01020304050607"));
	request1->setModuleID(0);
	request1->setValue(3.5);
	request1->setTimeout(Timespan(10, 0));
	request1->setMode(OpMode::REPEAT_ON_FAIL);

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type" : "set_value_request",
			"id" : "4a41d041-eb1e-4e9c-9528-1bbe74f54d59",
			"device_id" : "0xfe01020304050607",
			"mode" : "repeat_on_fail",
			"module_id" : 0,
			"value" : 3.5,
			"timeout" : 10
		})"),
		request1->toString()
	);

	GWSetValueRequest::Ptr request2(new GWSetValueRequest);
	CPPUNIT_ASSERT_THROW(request2->setValue(NAN), InvalidArgumentException);
}

void GWMessageTest::testParseDeviceAccept()
{
	GWMessage::Ptr message = GWMessage::fromJSON(
	R"({
			"message_type": "device_accept_request",
			"id": "4a41d041-eb1e-4e9c-9528-1bbe74f54d59",
			"device_id": "0xfe01020304050607"
	})");

	CPPUNIT_ASSERT_EQUAL(GWMessageType::DEVICE_ACCEPT_REQUEST, message->type().raw());
	CPPUNIT_ASSERT(!message.cast<GWDeviceAcceptRequest>().isNull());

	GWDeviceAcceptRequest::Ptr request = message.cast<GWDeviceAcceptRequest>();
	CPPUNIT_ASSERT_EQUAL("4a41d041-eb1e-4e9c-9528-1bbe74f54d59", request->id().toString());
	CPPUNIT_ASSERT_EQUAL("0xfe01020304050607", request->deviceID().toString());
	CPPUNIT_ASSERT(request->refresh().isNone());
}

void GWMessageTest::testCreateDeviceAccept()
{
	GWDeviceAcceptRequest::Ptr request(new GWDeviceAcceptRequest);
	request->setID(GlobalID::parse("4a41d041-eb1e-4e9c-9528-1bbe74f54d59"));
	request->setDeviceID(DeviceID::parse("0xfe01020304050607"));
	request->setRefresh(RefreshTime::fromSeconds(56));

	CPPUNIT_ASSERT_EQUAL(
		jsonReformat(R"({
			"message_type": "device_accept_request",
			"id": "4a41d041-eb1e-4e9c-9528-1bbe74f54d59",
			"device_id": "0xfe01020304050607",
			"refresh_time": "56"
		})"),
		request->toString()
	);
}

}
