#include "gwmessage/GWGatewayAccepted.h"

using namespace Poco;
using namespace BeeeOn;

GWGatewayAccepted::GWGatewayAccepted():
	GWMessage(GWMessageType::GATEWAY_ACCEPTED)
{
}

GWGatewayAccepted::GWGatewayAccepted(const JSON::Object::Ptr object):
	GWMessage(object)
{
}
