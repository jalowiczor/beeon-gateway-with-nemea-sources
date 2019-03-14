#include <string>

#include <Poco/Exception.h>
#include <Poco/NumberParser.h>

#include "gwmessage/GWSearchRequest.h"

using namespace std;
using namespace Poco;
using namespace Poco::JSON;
using namespace Poco::Net;
using namespace BeeeOn;

GWSearchRequest::GWSearchRequest():
	GWRequest(GWMessageType::SEARCH_REQUEST)
{
}

GWSearchRequest::GWSearchRequest(const Object::Ptr object):
	GWRequest(object)
{
}

void GWSearchRequest::setDuration(const Timespan &duration)
{
	json()->set("duration", duration.totalSeconds());
}

Timespan GWSearchRequest::duration() const
{
	return json()->getValue<unsigned>("duration") * Timespan::SECONDS;
}

void GWSearchRequest::setDevicePrefix(const DevicePrefix &prefix)
{
	json()->set("device_prefix", prefix.toString());
}

DevicePrefix GWSearchRequest::devicePrefix() const
{
	return DevicePrefix::parse(json()->getValue<string>("device_prefix"));
}

void GWSearchRequest::setCriteria(const DeviceCriteria &criteria)
{
	Object::Ptr object = new Object;

	if (criteria.hasIPAddress()) {
		object->set("ip_address", criteria.ipAddress().toString());
	}
	else if (criteria.hasMACAddress()) {
		object->set("mac_address", criteria.macAddress().toString(':'));
	}
	else if (criteria.hasSerialNumber()) {
		object->set("serial_number", to_string(criteria.serialNumber()));
	}
	else {
		throw IllegalStateException("invalid criteria for device-search, no rules");
	}

	json()->set("criteria", object);
}

DeviceCriteria GWSearchRequest::criteria() const
{
	Object::Ptr object = json()->getObject("criteria");

	if (object->has("ip_address")) {
		const IPAddress address(object->getValue<string>("ip_address"));
		return DeviceCriteria(address);
	}
	else if (object->has("mac_address")) {
		const auto address = MACAddress::parse(object->getValue<string>("mac_address"));
		return DeviceCriteria(address);
	}
	else if (object->has("serial_number")) {
		const uint64_t serial = NumberParser::parseUnsigned(
				object->getValue<string>("serial_number"));
		return DeviceCriteria(serial);
	}
	else {
		throw IllegalStateException("missing criteria rules for device-search");
	}
}
