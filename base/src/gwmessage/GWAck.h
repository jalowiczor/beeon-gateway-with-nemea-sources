#pragma once

#include <Poco/JSON/Object.h>

#include "gwmessage/GWMessage.h"
#include "gwmessage/GWResponse.h"

namespace BeeeOn {

/**
 * @brief Represents an acknowledgement, that a response of the given id
 * and status has been really delivered. It is intended to handle unreliable
 * network connections.
 *
 * There can be more responses with the same id, so status is also needed
 * to identify and confirm proper response.
 *
 * An example of an ack message:
 * <pre>
 * {
 *   "message_type": "generic_ack",
 *   "id": "4c42288f-d592-4d6a-b401-0c2ef47c6d6d",
 *   "status": 0
 * }
 * </pre>
 */
class GWAck : public GWMessage {
public:
	typedef Poco::SharedPtr<GWAck> Ptr;

	GWAck();
	GWAck(const Poco::JSON::Object::Ptr object);

protected:
	GWAck(const GWMessageTypeEnum::Raw &type);

public:
	/**
	 * @brief Set a status intended to match a proper response.
	 */
	void setStatus(GWResponse::Status status);

	/**
	 * @brief Returns a status intended to match a proper response.
	 */
	GWResponse::Status status() const;
};

}
