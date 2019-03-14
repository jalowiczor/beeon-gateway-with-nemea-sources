#include "gwmessage/GWDeviceListRequest.h"
#include "gwmessage/GWDeviceListResponse.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

GWDeviceListRequest::GWDeviceListRequest():
	GWRequest(GWMessageType::DEVICE_LIST_REQUEST)
{
}

GWDeviceListRequest::GWDeviceListRequest(const JSON::Object::Ptr object) :
	GWRequest(object)
{
}

void GWDeviceListRequest::setDevicePrefix(const DevicePrefix &prefix)
{
	json()->set("device_prefix", prefix.toString());
}

DevicePrefix GWDeviceListRequest::devicePrefix() const
{
	return DevicePrefix::parse(json()->getValue<string>("device_prefix"));
}

GWResponse::Ptr GWDeviceListRequest::deriveResponse() const
{
	GWDeviceListResponse::Ptr response(new GWDeviceListResponse);
	return deriveGenericResponse(response);
}
