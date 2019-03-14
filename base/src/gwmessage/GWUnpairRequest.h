#pragma once

#include <Poco/SharedPtr.h>
#include <Poco/JSON/Object.h>

#include "gwmessage/GWRequest.h"
#include "model/DeviceID.h"

namespace BeeeOn {

/**
 * @brief Represents a message sent by the server to the gateway, intended
 * to unpair a paired device from the gateway.
 *
 * An example message:
 * <pre>
 * {
 *   "id": "fe2ba7e3-5d93-4c11-bc36-8f0b2f0a7b1a",
 *   "message_type": "unpair_request",
 *   "device_id": "0xa300000000000002"
 * }
 * </pre>
 */
class GWUnpairRequest : public GWRequest {
public:
	typedef Poco::SharedPtr<GWUnpairRequest> Ptr;

	GWUnpairRequest();
	GWUnpairRequest(const Poco::JSON::Object::Ptr object);

	void setDeviceID(const DeviceID &deviceID);
	DeviceID deviceID() const;

protected:
	GWResponse::Ptr deriveResponse() const override;
};

}
