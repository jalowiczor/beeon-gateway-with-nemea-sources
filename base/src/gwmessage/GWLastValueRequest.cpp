#include "gwmessage/GWLastValueRequest.h"
#include "gwmessage/GWLastValueResponse.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

GWLastValueRequest::GWLastValueRequest():
	GWRequest(GWMessageType::LAST_VALUE_REQUEST)
{
}

GWLastValueRequest::GWLastValueRequest(const JSON::Object::Ptr object) :
	GWRequest(object)
{
}

void GWLastValueRequest::setDeviceID(const DeviceID &id)
{
	json()->set("device_id", id.toString());
}

DeviceID GWLastValueRequest::deviceID() const
{
	return DeviceID::parse(json()->getValue<string>("device_id"));
}

void GWLastValueRequest::setModuleID(const ModuleID &id)
{
	json()->set("module_id", id.toString());
}

ModuleID GWLastValueRequest::moduleID() const
{
	return ModuleID::parse(json()->getValue<string>("module_id"));
}

GWResponse::Ptr GWLastValueRequest::deriveResponse() const
{
	GWLastValueResponse::Ptr response(new GWLastValueResponse);
	return deriveGenericResponse(response);
}
