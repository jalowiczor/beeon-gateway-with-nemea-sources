#pragma once

#include <map>

#include <Poco/SharedPtr.h>
#include <Poco/JSON/Object.h>

#include "gwmessage/GWRequest.h"
#include "model/DeviceID.h"
#include "model/RefreshTime.h"
#include "util/Loggable.h"

namespace BeeeOn {

/**
 * @brief Represents a message sent by the server to the gateway, intended
 * to inform gateway that the user accepted the discovered device. It means
 * request to pair the device with the gateway.
 *
 * Optionally, the refresh time can be configured by this command. Also,
 * the section "config" can contain certain configuration entries that might
 * help to initialize the device.
 *
 * An example message:
 * <pre>
 * {
 *   "id": "603a5085-1b00-4c5e-ac70-dde674a49b0c",
 *   "message_type": "device_accept_request",
 *   "device_id": "0xa300000000000001"
 *   "refresh_time": "15",
 *   "config": {
 *      "ip-address": "192.168.0.1"
 *   }
 * }
 * </pre>
 */
class GWDeviceAcceptRequest : public GWRequest, Loggable {
public:
	typedef Poco::SharedPtr<GWDeviceAcceptRequest> Ptr;

	GWDeviceAcceptRequest();
	GWDeviceAcceptRequest(const Poco::JSON::Object::Ptr object);

	void setDeviceID(const DeviceID &deviceID);
	DeviceID deviceID() const;

	void setRefresh(const RefreshTime &refresh);
	RefreshTime refresh() const;

	void setProperties(
		const std::map<std::string, std::string> &properties);
	std::map<std::string, std::string> properties() const;
};

}
