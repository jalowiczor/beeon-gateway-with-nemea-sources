#pragma once

#include <Poco/SharedPtr.h>
#include <Poco/JSON/Object.h>

#include "gwmessage/GWRequest.h"
#include "model/DevicePrefix.h"

namespace BeeeOn {

/**
 * @brief Represents a message sent by the gateway to the server, intended
 * to requests a list of paired devices with a specific prefix.
 *
 * GWDeviceListRequest represents response to this request.
 *
 * An example message:
 * <pre>
 * {
 *   "id": "ffd0da59-5f6d-4363-b2ad-afa95ef85d59",
 *   "message_type": "device_list_request",
 *   "device_prefix": "vdev"
 * }
 * </pre>
 */
class GWDeviceListRequest : public GWRequest {
public:
	typedef Poco::SharedPtr<GWDeviceListRequest> Ptr;

	GWDeviceListRequest();
	GWDeviceListRequest(const Poco::JSON::Object::Ptr object);

	void setDevicePrefix(const DevicePrefix &prefix);
	DevicePrefix devicePrefix() const;

protected:
	GWResponse::Ptr deriveResponse() const override;
};

}
