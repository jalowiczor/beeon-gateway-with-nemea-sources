#include "gwmessage/GWGatewayRegister.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;
using namespace BeeeOn;

GWGatewayRegister::GWGatewayRegister():
	GWMessage(GWMessageType::GATEWAY_REGISTER)
{
}

GWGatewayRegister::GWGatewayRegister(const JSON::Object::Ptr object):
	GWMessage(object)
{
}

void GWGatewayRegister::setGatewayID(const GatewayID &gatewayID)
{
	json()->set("gateway_id", gatewayID.toString());
}

GatewayID GWGatewayRegister::gatewayID() const
{
	return GatewayID::parse(json()->getValue<string>("gateway_id"));
}

void GWGatewayRegister::setVersion(const std::string &version)
{
	json()->set("version", version);
}

string GWGatewayRegister::version() const
{
	return json()->getValue<string>("version");
}

void GWGatewayRegister::setIPAddress(const Poco::Net::IPAddress &ipAddress)
{
	json()->set("ip_address", ipAddress.toString());
}

IPAddress GWGatewayRegister::ipAddress() const
{
	return IPAddress::parse(json()->getValue<string>("ip_address"));
}
