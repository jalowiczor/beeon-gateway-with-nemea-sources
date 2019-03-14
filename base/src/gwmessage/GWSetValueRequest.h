#pragma once

#include <Poco/SharedPtr.h>
#include <Poco/Timespan.h>
#include <Poco/JSON/Object.h>

#include "gwmessage/GWRequest.h"
#include "model/DeviceID.h"
#include "model/ModuleID.h"
#include "model/OpMode.h"

namespace BeeeOn {

/**
 * @brief Represents a message sent by the server to the gateway, intended
 * to change state of an active module on the device.
 *
 * The message contains an identification of the device and its module,
 * a value to be set and a timeout for which the value must be set
 * or the operation end with failed.
 *
 * An example message:
 * <pre>
 * {
 *   "id": "a507755f-793f-4067-89bf-533fb73b6a41",
 *   "message_type": "set_value_request",
 *   "device_id": "0xa300000000000001",
 *   "module_id": "0",
 *   "timeout": 30,
 *   "value": 1.0
 * }
 * </pre>
 */
class GWSetValueRequest : public GWRequest {
public:
	typedef Poco::SharedPtr<GWSetValueRequest> Ptr;

	GWSetValueRequest();
	GWSetValueRequest(const Poco::JSON::Object::Ptr object);

	void setDeviceID(const DeviceID &deviceID);
	DeviceID deviceID() const;

	void setModuleID(const ModuleID &moduleID);
	ModuleID moduleID() const;

	void setValue(double value);
	double value() const;

	void setTimeout(const Poco::Timespan &timeout);
	Poco::Timespan timeout() const;

	void setMode(const OpMode &mode);
	OpMode mode() const;
};

}
