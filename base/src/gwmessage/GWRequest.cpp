#include "gwmessage/GWRequest.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

GWRequest::GWRequest(const GWMessageType::Raw &type):
	GWMessage(type)
{
}

GWRequest::GWRequest(const JSON::Object::Ptr object):
	GWMessage(object)
{
}

GWResponse::Ptr GWRequest::deriveGenericResponse(GWResponse::Ptr response) const
{
	response->setID(id());
	return response;
}

GWResponse::Ptr GWRequest::deriveResponse() const
{
	return deriveGenericResponse(new GWResponse);
}
