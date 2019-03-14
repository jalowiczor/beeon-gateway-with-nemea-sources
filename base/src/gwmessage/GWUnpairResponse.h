#pragma once

#include <set>

#include "gwmessage/GWResponse.h"
#include "model/DeviceID.h"

namespace BeeeOn {

/**
 * @brief The unpair process might under certain circumstances unpair
 * a different device then intended. The custom unpair response holds
 * a list of IDs of devices unpaired by the unpair request. Depending
 * on the server, it might be possible to use also generic GWResponse.
 * But this class is better and should be preferred.
 *
 * An example message:
 * <pre>
 * {
 *   "id": "603a5085-1b00-4c5e-ac70-dde674a49b0c",
 *   "message_type": "unpair_response",
 *   "devices": {
 *      {"device_id": "0xa1012345aabbccdd"}
 *   }
 * }
 * </pre>
 */
class GWUnpairResponse : public GWResponse {
public:
	GWUnpairResponse();
	GWUnpairResponse(const Poco::JSON::Object::Ptr object);

	void setUnpairedDevices(const std::set<DeviceID> &devices);
	std::set<DeviceID> unpairedDevices() const;
};

}
