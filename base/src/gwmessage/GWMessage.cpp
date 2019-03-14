#include "gwmessage/GWMessage.h"
#include "gwmessage/GWDeviceAcceptRequest.h"
#include "gwmessage/GWDeviceListRequest.h"
#include "gwmessage/GWDeviceListResponse.h"
#include "gwmessage/GWGatewayRegister.h"
#include "gwmessage/GWGatewayAccepted.h"
#include "gwmessage/GWLastValueRequest.h"
#include "gwmessage/GWLastValueResponse.h"
#include "gwmessage/GWListenRequest.h"
#include "gwmessage/GWNewDeviceGroupRequest.h"
#include "gwmessage/GWNewDeviceRequest.h"
#include "gwmessage/GWNoticeRequest.h"
#include "gwmessage/GWResponse.h"
#include "gwmessage/GWAck.h"
#include "gwmessage/GWResponseWithAck.h"
#include "gwmessage/GWSearchRequest.h"
#include "gwmessage/GWSensorDataConfirm.h"
#include "gwmessage/GWSensorDataExport.h"
#include "gwmessage/GWSetValueRequest.h"
#include "gwmessage/GWUnpairRequest.h"
#include "gwmessage/GWUnpairResponse.h"
#include "util/JsonUtil.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

const string MESSAGE_TYPE_KEY = "message_type";

GWMessage::GWMessage(const GWMessageType::Raw &type):
	m_json(new JSON::Object)
{
	setType(type);
}

GWMessage::GWMessage(const JSON::Object::Ptr object):
	m_json(object)
{
}

GWMessage::~GWMessage()
{
}

GlobalID GWMessage::id() const
{
	return GlobalID::parse(json()->getValue<string>("id"));
}

void GWMessage::setID(const GlobalID &id)
{
	json()->set("id", id.toString());
}

GWMessageType GWMessage::type() const
{
	return GWMessageType::parse(m_json->getValue<string>(MESSAGE_TYPE_KEY));
}

std::string GWMessage::toString() const
{
	return Dynamic::Var::toString(m_json);
}

string GWMessage::toBriefString() const
{
	const auto id = json()->has("id")? this->id() : GlobalID();
	return type().toString() + " " + id.toString();
}

GWMessage::Ptr GWMessage::fromJSON(const std::string &json)
{
	return fromJSON(JsonUtil::parse(json));
}

GWMessage::Ptr GWMessage::fromJSON(Poco::JSON::Object::Ptr object)
{
	const GWMessageType &type = GWMessageType::parse(
		object->getValue<string>(MESSAGE_TYPE_KEY)
	);

	switch(type.raw()) {
	case GWMessageType::DEVICE_ACCEPT_REQUEST:
		return new GWDeviceAcceptRequest(object);
	case GWMessageType::DEVICE_LIST_REQUEST:
		return new GWDeviceListRequest(object);
	case GWMessageType::DEVICE_LIST_RESPONSE:
		return new GWDeviceListResponse(object);
	case GWMessageType::GATEWAY_REGISTER:
		return new GWGatewayRegister(object);
	case GWMessageType::GATEWAY_ACCEPTED:
		return new GWGatewayAccepted(object);
	case GWMessageType::GENERIC_RESPONSE:
		return new GWResponse(object);
	case GWMessageType::GENERIC_ACK:
		return new GWAck(object);
	case GWMessageType::LAST_VALUE_REQUEST:
		return new GWLastValueRequest(object);
	case GWMessageType::LAST_VALUE_RESPONSE:
		return new GWLastValueResponse(object);
	case GWMessageType::LISTEN_REQUEST:
		return new GWListenRequest(object);
	case GWMessageType::NEW_DEVICE_GROUP_REQUEST:
		return new GWNewDeviceGroupRequest(object);
	case GWMessageType::NEW_DEVICE_REQUEST:
		return new GWNewDeviceRequest(object);
	case GWMessageType::RESPONSE_WITH_ACK:
		return new GWResponseWithAck(object);
	case GWMessageType::SEARCH_REQUEST:
		return new GWSearchRequest(object);
	case GWMessageType::SENSOR_DATA_CONFIRM:
		return new GWSensorDataConfirm(object);
	case GWMessageType::SENSOR_DATA_EXPORT:
		return new GWSensorDataExport(object);
	case GWMessageType::SET_VALUE_REQUEST:
		return new GWSetValueRequest(object);
	case GWMessageType::UNPAIR_REQUEST:
		return new GWUnpairRequest(object);
	case GWMessageType::UNPAIR_RESPONSE:
		return new GWUnpairResponse(object);
	case GWMessageType::NOTICE_REQUEST:
		return new GWNoticeRequest(object);
	}

	throw InvalidArgumentException(
		"unsupported message type " + type.toString());
}

JSON::Object::Ptr GWMessage::json() const
{
	return m_json;
}

void GWMessage::setType(const GWMessageType &type)
{
	m_json->set(MESSAGE_TYPE_KEY, type.toString());
}
