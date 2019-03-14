#pragma once

#include <Poco/SharedPtr.h>
#include <Poco/Timespan.h>
#include <Poco/JSON/Object.h>

#include "gwmessage/GWRequest.h"

namespace BeeeOn {

/**
 * @brief Represents a message sent by the server to the gateway, intended
 * to start 'listening mode' on the gateway. In this mode, gateway can discover
 * new devices from the sensoric network.
 *
 * The listening mode has typically a limited duration. After the duration
 * exceeds, the listening mode is automatically turned off.
 *
 * An example message:
 * <pre>
 * {
 *   "id": "c149f44a-ab44-459a-b18f-a5c33d3d3c3c",
 *   "message_type": "listen_request",
 *   "duration": 60
 * }
 * </pre>
 */
class GWListenRequest : public GWRequest {
public:
	typedef Poco::SharedPtr<GWListenRequest> Ptr;

	GWListenRequest();
	GWListenRequest(const Poco::JSON::Object::Ptr object);

	void setDuration(const Poco::Timespan &duration);
	Poco::Timespan duration() const;
};

}
