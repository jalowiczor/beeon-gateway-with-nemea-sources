#pragma once

#include <list>
#include <string>

#include <Poco/SharedPtr.h>
#include <Poco/Timespan.h>
#include <Poco/JSON/Object.h>

#include "gwmessage/GWRequest.h"
#include "model/DeviceDescription.h"
#include "model/DeviceID.h"
#include "model/ModuleType.h"

namespace BeeeOn {

/**
 * @brief Represents a message sent by the gateway to the server after
 * discovering new device in the sensoric network.
 *
 * The message contains unique device identification, its product name along
 * with vendor, refresh time and types of device modules.
 *
 * There are some optional properties each device can report about itself.
 * These are currently: "name" (name of the device instance, not product name),
 * "firmware" (string that informs about firmware version), "ip_address",
 * "mac_address", "serial_number".
 *
 * Response to this request is the generic response GWResponse.
 *
 * An example message:
 * <pre>
 * {
 *   "id": "87e63e94-4954-4600-9c37-348c7626d469",
 *   "message_type": "new_device_request",
 *   "device_id": "0xa100000000000001",
 *   "product_name": "Temperature Sensor",
 *   "vendor": "Brno University of Technology",
 *   "refresh_time": 30,
 *   "serial_number": "1234567",
 *   "firmware": "v125667",
 *   "module_types": [
 *      {
 *        "type": "temperature",
 *        "attributes": ["inner"]
 *      },
 *      {
 *        "type": "temperature",
 *        "attributes": ["outer"]
 *      },
 *      {
 *        "type": "battery",
 *        "attributes": []
 *      },
 *      {
 *        "type": "enum",
 *        "subtype": "BLINK_MODE",
 *        "attributes": ["controllable"]
 *      }
 *   ]
 * }
 * </pre>
 */
class GWNewDeviceRequest : public GWRequest {
public:
	typedef Poco::SharedPtr<GWNewDeviceRequest> Ptr;

	GWNewDeviceRequest();
	GWNewDeviceRequest(const Poco::JSON::Object::Ptr object);

	void setDeviceID(const DeviceID &deviceID);
	DeviceID deviceID() const;

	void setProductName(const std::string &name);
	std::string productName() const;

	void setVendor(const std::string &vendor);
	std::string vendor() const;

	void setModuleTypes(const std::list<ModuleType> &types);
	std::list<ModuleType> moduleTypes() const;

	void setRefreshTime(const Poco::Timespan &time);
	Poco::Timespan refreshTime() const;

	void setDeviceDescription(const DeviceDescription &description);
	DeviceDescription deviceDescription() const;

	static Poco::JSON::Array::Ptr serializeModuleTypes(
		const std::list<ModuleType> &types);
	static std::list<ModuleType> parseModuleTypes(
		const Poco::JSON::Array::Ptr arrayOfTypes);
};

}
