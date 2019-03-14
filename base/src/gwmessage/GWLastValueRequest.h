#pragma once

#include <Poco/SharedPtr.h>
#include <Poco/JSON/Object.h>

#include "gwmessage/GWRequest.h"
#include "model/DeviceID.h"
#include "model/ModuleID.h"

namespace BeeeOn {

/**
 * @brief **Deprecated.** Represents a message sent by the gateway to the server, intended
 * to requests a last known value of a device module.
 *
 * GWLastValueResponse represents response to this request.
 */
class GWLastValueRequest : public GWRequest {
public:
	typedef Poco::SharedPtr<GWLastValueRequest> Ptr;

	GWLastValueRequest();
	GWLastValueRequest(const Poco::JSON::Object::Ptr object);

	void setDeviceID(const DeviceID &id);
	DeviceID deviceID() const;

	void setModuleID(const ModuleID &id);
	ModuleID moduleID() const;

protected:
	GWResponse::Ptr deriveResponse() const override;
};

}
