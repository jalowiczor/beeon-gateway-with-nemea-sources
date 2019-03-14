#pragma once

#include <map>
#include <vector>

#include <Poco/SharedPtr.h>
#include <Poco/JSON/Array.h>
#include <Poco/JSON/Object.h>

#include "gwmessage/GWResponse.h"
#include "model/DeviceID.h"
#include "model/ModuleID.h"
#include "model/RefreshTime.h"
#include "util/Loggable.h"

namespace BeeeOn {

/**
 * @brief Represents a message sent byt the server to the gateway as a response
 * to the GWDeviceListRequest.
 *
 * The message contains a list of paired devices with a specific prefix.
 *
 * For each device, there is an optional section under key _config_.
 * Here, some configuration options might be available. Currently,
 * the well known keys are: "refresh_time", "ip_address", "password".
 *
 * An example message:
 * <pre>
 * {
 *   "id": "60775a50-d91c-4325-89b1-283e38bd60b2",
 *   "message_type": "device_list_response",
 *   "status": 1,
 *   "devices": [
 *       {"device_id": "0xa300000000000001"}
 *   ],
 *   "values": {
 *       "0xa300000000000001": {
 *           "0": 0,
 *           "1": 10.0
 *       }
 *   },
 *   "config": {
 *      "0xa300000000000001": {
 *         "regresh_time": "30",
 *         "password": "super-secret",
 *         "ip-address": "10.0.0.1"
 *      }
 *   }
 * }
 * </pre>
 */
class GWDeviceListResponse : public GWResponse, Loggable {
public:
	typedef Poco::SharedPtr<GWDeviceListResponse> Ptr;

	GWDeviceListResponse();
	GWDeviceListResponse(const Poco::JSON::Object::Ptr object);

	void setDevices(const std::vector<DeviceID> &devices);

	/**
	 * @returns device IDs of paired devices
	 */
	std::vector<DeviceID> devices() const;

	void setModulesValues(
		const DeviceID &device,
		const std::map<ModuleID, double> &values);

	/**
	 * @returns map of the most recent values for the given device
	 */
	std::map<ModuleID, double> modulesValues(const DeviceID &device) const;

	void setRefreshFor(const DeviceID &device, const RefreshTime &refresh);
	RefreshTime refreshFor(const DeviceID &device) const;

	void setProperties(
		const DeviceID &device,
		const std::map<std::string, std::string> &properties);
	std::map<std::string, std::string> properties(
		const DeviceID &device) const;
};

}
