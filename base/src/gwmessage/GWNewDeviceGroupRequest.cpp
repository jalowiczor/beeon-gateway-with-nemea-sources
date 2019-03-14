#include <list>

#include <Poco/Timespan.h>
#include <Poco/JSON/Array.h>

#include "gwmessage/GWNewDeviceGroupRequest.h"
#include "gwmessage/GWNewDeviceRequest.h"
#include "model/DeviceID.h"
#include "model/ModuleType.h"
#include "util/JsonUtil.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;
using namespace BeeeOn;

GWNewDeviceGroupRequest::GWNewDeviceGroupRequest():
	GWRequest(GWMessageType::NEW_DEVICE_GROUP_REQUEST)
{
	JSON::Array::Ptr array(new JSON::Array);
	json()->set("devices", array);
}

GWNewDeviceGroupRequest::GWNewDeviceGroupRequest(const JSON::Object::Ptr object):
	GWRequest(object)
{
}

void GWNewDeviceGroupRequest::setGroupName(const std::string& groupName)
{
	json()->set("group_name", groupName);
}

string GWNewDeviceGroupRequest::groupName() const
{
	return json()->getValue<string>("group_name");
}

void GWNewDeviceGroupRequest::setVendor(const std::string& vendor)
{
	json()->set("vendor", vendor);
}

string GWNewDeviceGroupRequest::vendor() const
{
	return json()->getValue<string>("vendor");
}

void GWNewDeviceGroupRequest::addDeviceDescription(const DeviceDescription& deviceDescription)
{
	JSON::Object::Ptr newDevice(new JSON::Object);

	newDevice->set("device_id", deviceDescription.id().toString());
	newDevice->set("product_name", deviceDescription.productName());
	newDevice->set("module_types",
		GWNewDeviceRequest::serializeModuleTypes(deviceDescription.dataTypes()));

	if (!deviceDescription.name().empty())
		json()->set("name", deviceDescription.name());
	if (!deviceDescription.firmware().empty())
		json()->set("firmware", deviceDescription.firmware());
	if (!deviceDescription.ipAddress().isNull())
		json()->set("ip_address", deviceDescription.ipAddress().value().toString());
	if (!deviceDescription.macAddress().isNull())
		json()->set("mac_address", deviceDescription.macAddress().value().toString(':'));
	if (!deviceDescription.serialNumber().isNull())
		json()->set("serial_number", to_string(deviceDescription.serialNumber().value()));

	newDevice->set("refresh_time", deviceDescription.refreshTime().seconds());
	json()->getArray("devices")->add(Dynamic::Var(newDevice));
}

vector<DeviceDescription> GWNewDeviceGroupRequest::deviceDescriptions() const
{
	vector<DeviceDescription> descriptions;
	JSON::Array::Ptr array = json()->getArray("devices");

	for (uint32_t i = 0; i < array->size(); i++) {
		JSON::Object::Ptr newDevice = array->getObject(i);
		JSON::Array::Ptr arrayOfTypes = newDevice->getArray("module_types");

		auto builder = DeviceDescription::Builder()
			.id(DeviceID::parse(newDevice->getValue<string>("device_id")))
			.type(vendor(), newDevice->getValue<string>("product_name"))
			.modules(GWNewDeviceRequest::parseModuleTypes(arrayOfTypes));

		const int time = newDevice->getValue<int>("refresh_time");
		if (time < 0)
			builder.noRefreshTime();
		else if (time == 0)
			builder.disabledRefreshTime();
		else
			builder.refreshTime(time * Timespan::SECONDS);

		if (json()->has("name"))
			builder.name(json()->getValue<string>("name"));
		if (json()->has("firmware"))
			builder.firmware(json()->getValue<string>("firmware"));

		if (json()->has("ip_address")) {
			builder.ipAddress(
				IPAddress::parse(
					json()->getValue<string>("ip_address")));
		}

		if (json()->has("mac_address")) {
			builder.macAddress(
				MACAddress::parse(
					json()->getValue<string>("mac_address")));
		}

		if (json()->has("serial_number")) {
			builder.serialNumber(
				NumberParser::parseUnsigned64(
					json()->getValue<string>("serial_number")));
		}

		descriptions.emplace_back(builder.build());
	}

	return descriptions;
}
