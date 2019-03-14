#pragma once

#include <Poco/SharedPtr.h>
#include <Poco/JSON/Object.h>

#include "gwmessage/GWResponse.h"

namespace BeeeOn {

/**
 * @brief **Deprecated.** Represents a response to a request message
 * (subclass of a GWRequest), that requires acknowledgement (GWAck).
 * In the future, just use a regular GWResponse and set ackRequired
 * property to true.
 */
class GWResponseWithAck : public GWResponse {
public:
	typedef Poco::SharedPtr<GWResponseWithAck> Ptr;

	GWResponseWithAck();
	GWResponseWithAck(const Poco::JSON::Object::Ptr object);

protected:
	GWResponseWithAck(const GWMessageTypeEnum::Raw &type);
};

}
