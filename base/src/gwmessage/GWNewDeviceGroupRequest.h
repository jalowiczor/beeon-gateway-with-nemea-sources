#pragma once

#include <vector>

#include <Poco/SharedPtr.h>
#include <Poco/JSON/Object.h>

#include "gwmessage/GWRequest.h"
#include "model/DeviceDescription.h"

namespace BeeeOn {

/**
 * @brief Represents a message sent by the gateway to the server after
 * discovering a device consisting of a group of subdevices in the sensoric
 * network. Example of the device may be VPT regulator that consists of
 * 4 zones and a boiler. Vendor name stored in message has to be same
 * for all devices.
 *
 * Response to this request is the generic response GWResponse.
 *
 * An example message:
 * <pre>
 * {
 *   "message_type": "new_device_group_request",
 *   "id": "bddc7e1d-2578-40c0-8d42-43f818fef401",
 *   "group_name": "Control Center",
 *   "vendor": "Example Control Company",
 *   "devices": [
 *     {
 *       "device_id": "0xa100000000000001",
 *       "product_name": "Control Panel"
 *       "module_types": [
 *         {
 *           "type": "on_off",
 *           "attributes": ["controllable"]
 *         },
 *         {
 *           "type": "on_off",
 *           "attributes": ["controllable"]
 *         }
 *       ],
 *       "refresh_time": -1
 *     },
 *     {
 *       "device_id": "0xa100000000000002",
 *       "product_name": "Probe",
 *       "module_types": [
 *         {
 *           "type": "temperature",
 *           "attributes": ["inner"]
 *         },
 *         {
 *           "type": "humidity",
 *           "attributes": ["inner"]
 *         }
 *       ],
 *       "refresh_time": 120
 *     }
 *   ]
 * }
 * </pre>
 */
class GWNewDeviceGroupRequest : public GWRequest {
public:
	typedef Poco::SharedPtr<GWNewDeviceGroupRequest> Ptr;

	GWNewDeviceGroupRequest();
	GWNewDeviceGroupRequest(const Poco::JSON::Object::Ptr object);

	void setGroupName(const std::string& groupName);
	std::string groupName() const;

	void setVendor(const std::string& vendor);
	std::string vendor() const;

	void addDeviceDescription(const DeviceDescription& deviceDescription);
	std::vector<DeviceDescription> deviceDescriptions() const;
};

}
