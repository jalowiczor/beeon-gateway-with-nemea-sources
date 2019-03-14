#include "gwmessage/GWUnpairRequest.h"
#include "gwmessage/GWUnpairResponse.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

GWUnpairRequest::GWUnpairRequest():
	GWRequest(GWMessageType::UNPAIR_REQUEST)
{
}

GWUnpairRequest::GWUnpairRequest(const JSON::Object::Ptr object) :
	GWRequest(object)
{
}

void GWUnpairRequest::setDeviceID(const DeviceID &deviceID)
{
	json()->set("device_id", deviceID.toString());
}

DeviceID GWUnpairRequest::deviceID() const
{
	return DeviceID::parse(json()->getValue<string>("device_id"));
}

GWResponse::Ptr GWUnpairRequest::deriveResponse() const
{
	GWUnpairResponse::Ptr response = new GWUnpairResponse;
	return deriveGenericResponse(response);
}
