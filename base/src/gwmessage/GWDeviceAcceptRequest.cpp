#include <Poco/Logger.h>

#include "gwmessage/GWDeviceAcceptRequest.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

GWDeviceAcceptRequest::GWDeviceAcceptRequest():
	GWRequest(GWMessageType::DEVICE_ACCEPT_REQUEST)
{
}

GWDeviceAcceptRequest::GWDeviceAcceptRequest(const JSON::Object::Ptr object):
	GWRequest(object)
{
}

void GWDeviceAcceptRequest::setDeviceID(const DeviceID &deviceID)
{
	json()->set("device_id", deviceID.toString());
}

DeviceID GWDeviceAcceptRequest::deviceID() const
{
	return DeviceID::parse(json()->getValue<string>("device_id"));
}

void GWDeviceAcceptRequest::setRefresh(const RefreshTime &refresh)
{
	if (refresh.isNone())
		return;

	json()->set("refresh_time", refresh.toString());
}

RefreshTime GWDeviceAcceptRequest::refresh() const
{
	return RefreshTime::parse(json()->optValue<string>("refresh_time", "none"));
}

void GWDeviceAcceptRequest::setProperties(
	const map<string, string> &properties)
{
	JSON::Object::Ptr config = json()->getObject("config");
	if (config.isNull()) {
		if (properties.empty())
			return;

		config = new JSON::Object;
		json()->set("config", config);
	}

	config->clear();

	for (const auto &pair : properties)
		config->set(pair.first, pair.second);
}

map<string, string> GWDeviceAcceptRequest::properties() const
{
	JSON::Object::Ptr config = json()->getObject("config");
	if (config.isNull())
		return {};

	map<string, string> properties;

	for (const auto &pair : *config) {
		string value;

		try {
			value = pair.second.convert<string>();
		}
		BEEEON_CATCH_CHAIN(logger())

		properties.emplace(pair.first, value);
	}

	return properties;
}
