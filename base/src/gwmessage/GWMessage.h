#pragma once

#include <string>

#include <Poco/SharedPtr.h>
#include <Poco/JSON/Object.h>

#include "gwmessage/GWMessageType.h"
#include "model/GlobalID.h"

namespace BeeeOn {

/**
 * @brief The GWMessage is abstract class representing messages (including
 * their contents), that are being sent between Gateway and Server.
 *
 * The internal representation of the GWMessage is the Poco::JSON::Object.
 * All subclasses should set the values directly into that instance
 * via accessing method json(). This leads to a lazy-validation behaviour.
 */
class GWMessage {
public:
	typedef Poco::SharedPtr<GWMessage> Ptr;

	virtual ~GWMessage();

	/**
	 * @brief Set unique identifier intended to match
	 * request message with its response.
	 */
	void setID(const GlobalID &id);

	/**
	 * @brief Return unique identifier intended to match
	 * request message with its response.
	 */
	GlobalID id() const;

protected:
	/**
	 * @brief Constructs a GWMessage depending on the given type.
	 *
	 * The type is set to the internal JSON::Object.
	 */
	GWMessage(const GWMessageType::Raw &type);

	/**
	 * @brief Constructs a GWMessage from the JSON::Object.
	 */
	GWMessage(const Poco::JSON::Object::Ptr object);

public:
	/**
	 * @brief Returns the type of the message.
	 */
	GWMessageType type() const;

	/**
	 * @brief Returns the string representation of the message.
	 */
	std::string toString() const;

	/**
	 * @returns brief representation of the message (useful for logging).
	 */
	std::string toBriefString() const;

	/**
	 * @brief Factory method for creating subclasses of the GWMessage
	 * from the json string.
	 *
	 * Type of the created message is determined by the message type
	 * in the json string.
	 */
	static GWMessage::Ptr fromJSON(const std::string &json);

	/**
	 * @brief Factory method for creating subclasses of the GWMessage
	 * from the JSON::Object.
	 *
	 * Type of the created message is determined by the message type
	 * in the JSON::Object.
	 */
	static GWMessage::Ptr fromJSON(Poco::JSON::Object::Ptr object);

protected:
	Poco::JSON::Object::Ptr json() const;

private:
	void setType(const GWMessageType &type);

private:
	Poco::JSON::Object::Ptr m_json;
};

}
