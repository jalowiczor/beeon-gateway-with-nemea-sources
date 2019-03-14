#pragma once

#include <Poco/SharedPtr.h>
#include <Poco/Timestamp.h>
#include <Poco/JSON/Object.h>

#include "gwmessage/GWRequest.h"
#include "model/Severity.h"

namespace BeeeOn {

/**
 * @brief GWNoticeRequest allows a gateway to send a notice
 * about something that happen. It can report progress information,
 * error details, hints, etc. Each notice is defined by the following
 * properties:
 *
 * - at - time when the message (or source event) has occoured
 * - severity - describes importance of the message: info, warn or error
 * - key - key that defines the message (intended to be translated to client)
 * - context - context that can be used to construct the client message
 *
 * All message should be designed with multi-locale environment in mind.
 * Thus, the key and context are kept separately and should be tranformed
 * to the appropriate user message on the presentation layer.
 *
 * An example message:
 * <pre>
 * {
 *   "id": "603a5085-1b00-4c5e-ac70-dde674a49b0c",
 *   "message_type": "notice_request",
 *   "at": 1546882536019124,
 *   "severity": "info",
 *   "key": "press_button_on_device",
 *   "context": {
 *     "device_id": "0xa3123456abcd01234",
 *     "gateway_id": "12437773180129",
 *     "button": "red"
 *   }
 * }
 * </pre>
 */
class GWNoticeRequest : public GWRequest {
public:
	typedef Poco::SharedPtr<GWNoticeRequest> Ptr;

	GWNoticeRequest();
	GWNoticeRequest(const Poco::JSON::Object::Ptr object);

	void setAt(const Poco::Timestamp &at);
	Poco::Timestamp at() const;

	void setSeverity(const Severity &severity);
	Severity severity() const;

	void setKey(const std::string &key);
	std::string key() const;

	void setContext(const Poco::JSON::Object::Ptr context);
	Poco::JSON::Object::Ptr context() const;
};

}
