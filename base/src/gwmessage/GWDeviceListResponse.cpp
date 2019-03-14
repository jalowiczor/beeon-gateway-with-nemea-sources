#include <Poco/Logger.h>

#include "gwmessage/GWDeviceListResponse.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

GWDeviceListResponse::GWDeviceListResponse():
	GWResponse(GWMessageType::DEVICE_LIST_RESPONSE)
{
}

GWDeviceListResponse::GWDeviceListResponse(const JSON::Object::Ptr object) :
	GWResponse(object)
{
}

void GWDeviceListResponse::setDevices(const vector<DeviceID> &devices)
{
	JSON::Array::Ptr array(new JSON::Array);

	for (const auto &id : devices) {
		JSON::Object::Ptr object(new JSON::Object);
		object->set("device_id", id.toString());
		array->add(object);
	}

	json()->set("devices", array);
}

vector<DeviceID> GWDeviceListResponse::devices() const
{
	vector<DeviceID> devices;
	JSON::Array::Ptr array = json()->getArray("devices");

	for (size_t i = 0; i < array->size(); i++) {
		JSON::Object::Ptr object = array->getObject(i);

		devices.push_back(DeviceID::parse(object->getValue<string>("device_id")));
	}

	return devices;
}

void GWDeviceListResponse::setModulesValues(
	const DeviceID &id,
	const map<ModuleID, double> &modulesValues)
{
	if (!json()->has("values") || !json()->isObject("values")) {
		JSON::Object::Ptr empty = new JSON::Object;
		json()->set("values", empty);
	}

	JSON::Object::Ptr values = json()->getObject("values");

	if (!values->has(id.toString()) || !values->isObject(id.toString())) {
		JSON::Object::Ptr empty = new JSON::Object;
		values->set(id.toString(), empty);
	}

	JSON::Object::Ptr device = values->getObject(id.toString());

	for (const auto &pair : modulesValues)
		device->set(pair.first.toString(), pair.second);
}

map<ModuleID, double> GWDeviceListResponse::modulesValues(
		const DeviceID &id) const
{
	map<ModuleID, double> result;

	JSON::Object::Ptr values = json()->getObject("values");
	if (values.isNull())
		return result;

	JSON::Object::Ptr device = values->getObject(id.toString());
	if (device.isNull())
		return result;

	for (const auto &pair : *device) {
		result.emplace(
			ModuleID::parse(pair.first),
			pair.second.convert<double>());
	}

	return result;
}

void GWDeviceListResponse::setRefreshFor(const DeviceID &id, const RefreshTime &refresh)
{
	if (refresh.isNone())
		return;

	JSON::Object::Ptr config = json()->getObject("config");
	if (config.isNull()) {
		config = new JSON::Object;
		json()->set("config", config);
	}

	JSON::Object::Ptr device = config->getObject(id.toString());
	if (device.isNull()) {
		device = new JSON::Object;
		config->set(id.toString(), device);
	}

	device->set("refresh_time", refresh.toString());
}

RefreshTime GWDeviceListResponse::refreshFor(const DeviceID &id) const
{
	JSON::Object::Ptr config = json()->getObject("config");
	if (config.isNull())
		return RefreshTime::NONE;

	JSON::Object::Ptr device = config->getObject(id.toString());
	if (device.isNull())
		return RefreshTime::NONE;

	return RefreshTime::parse(device->optValue<string>("refresh_time", "none"));
}

void GWDeviceListResponse::setProperties(
	const DeviceID &id,
	const map<string, string> &properties)
{
	JSON::Object::Ptr config = json()->getObject("config");
	if (config.isNull()) {
		if (properties.empty())
			return;

		config = new JSON::Object;
		json()->set("config", config);
	}

	JSON::Object::Ptr device = config->getObject(id.toString());
	if (device.isNull()) {
		if (properties.empty())
			return;

		device = new JSON::Object;
		config->set(id.toString(), device);
	}

	// first delete all, then set new
	const auto refresh = device->optValue<string>("refresh_time", "none");
	device->clear();

	if (refresh != "none")
		device->set("refresh_time", refresh);

	for (const auto &property : properties)
		device->set(property.first, property.second);
}

map<string, string> GWDeviceListResponse::properties(
	const DeviceID &id) const
{
	JSON::Object::Ptr config = json()->getObject("config");
	if (config.isNull())
		return {};

	JSON::Object::Ptr device = config->getObject(id.toString());
	if (device.isNull())
		return {};

	map<string, string> properties;

	for (const auto &pair : *device) {
		if (pair.first == "refresh_time")
			continue;

		string value;

		try {
			value = pair.second.convert<string>();
		}
		BEEEON_CATCH_CHAIN(logger())

		properties.emplace(pair.first, value);
	}

	return properties;
}
