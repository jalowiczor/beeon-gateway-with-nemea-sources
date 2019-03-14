#include "gwmessage/GWMessageType.h"
#include "gwmessage/GWResponseWithAck.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

GWResponseWithAck::GWResponseWithAck():
	GWResponse(GWMessageType::RESPONSE_WITH_ACK)
{
	setAckExpected(true);
}

GWResponseWithAck::GWResponseWithAck(const GWMessageType::Raw &type):
	GWResponse(type)
{
	setAckExpected(true);
}

GWResponseWithAck::GWResponseWithAck(const JSON::Object::Ptr object):
	GWResponse(object)
{
}
