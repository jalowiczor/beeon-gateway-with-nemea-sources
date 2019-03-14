#include "gwmessage/GWListenRequest.h"

using namespace Poco;
using namespace BeeeOn;

GWListenRequest::GWListenRequest():
	GWRequest(GWMessageType::LISTEN_REQUEST)
{
}

GWListenRequest::GWListenRequest(const JSON::Object::Ptr object):
	GWRequest(object)
{
}

void GWListenRequest::setDuration(const Timespan &duration)
{
	json()->set("duration", duration.totalSeconds());
}

Timespan GWListenRequest::duration() const
{
	return json()->getValue<unsigned>("duration") * Timespan::SECONDS;
}
