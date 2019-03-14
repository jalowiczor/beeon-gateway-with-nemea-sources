#pragma once

#include <Poco/SharedPtr.h>
#include <Poco/Timespan.h>
#include <Poco/JSON/Object.h>

#include "gwmessage/GWRequest.h"
#include "model/DeviceCriteria.h"
#include "model/DevicePrefix.h"

namespace BeeeOn {

/**
 * @brief This message is used to request searching for a device
 * by criteria. This is a kind of a discovery process where it is
 * known that a certain device has an exact identification. The
 * search is always intended for 1 IoT technology.
 *
 * Example message:
 * <pre>
 * {
 *   "message_type": "search_request",
 *   "id": "b3f83259-8acf-486d-9ab6-cf74b74ae311",
 *   "duration": 30,
 *   "device_prefix": "jablotron",
 *   "criteria": {
 *     "serial_number": "1497923740923"
 *   }
 * }
 * </pre>
 */
class GWSearchRequest : public GWRequest {
public:
	typedef Poco::SharedPtr<GWSearchRequest> Ptr;

	GWSearchRequest();
	GWSearchRequest(const Poco::JSON::Object::Ptr object);

	void setDuration(const Poco::Timespan &duration);
	Poco::Timespan duration() const;

	void setDevicePrefix(const DevicePrefix &prefix);
	DevicePrefix devicePrefix() const;

	void setCriteria(const DeviceCriteria &criteria);
	DeviceCriteria criteria() const;
};

}
