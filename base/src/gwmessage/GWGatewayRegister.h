#pragma once

#include <string>

#include <Poco/SharedPtr.h>
#include <Poco/JSON/Object.h>
#include <Poco/Net/IPAddress.h>

#include "gwmessage/GWMessage.h"
#include "model/GatewayID.h"

namespace BeeeOn {

/**
 * @brief Represents message sent by the gateway after connecting to the server.
 * Message is intended to gateway registration on the server.
 *
 * Server confirms successful registration with message GWGatewayAccepted.
 *
 * An example of a registration message:
 * <pre>
 * {
 *   "message_type": "gateway_register",
 *   "gateway_id": "1875034586645818",
 *   "version": "v2017.01",
 *   "ip_address": "192.168.0.1"
 * }
 * </pre>
 */
class GWGatewayRegister : public GWMessage {
public:
	typedef Poco::SharedPtr<GWGatewayRegister> Ptr;

	GWGatewayRegister();
	GWGatewayRegister(const Poco::JSON::Object::Ptr object);

	void setGatewayID(const GatewayID &gatewayID);

	/**
	 * @returns ID of the gateway that is being registered
	 */
	GatewayID gatewayID() const;

	void setVersion(const std::string &version);

	/**
	 * @returns version of the gateway software
	 */
	std::string version() const;

	void setIPAddress(const Poco::Net::IPAddress &ipAddress);

	/**
	 * @returns IP address the gateway can see on the socket used
	 * for communication with the remote server
	 */
	Poco::Net::IPAddress ipAddress() const;
};

}
