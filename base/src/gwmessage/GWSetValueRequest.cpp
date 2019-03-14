#include <cmath>

#include "gwmessage/GWSetValueRequest.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

GWSetValueRequest::GWSetValueRequest():
	GWRequest(GWMessageType::SET_VALUE_REQUEST)
{
}

GWSetValueRequest::GWSetValueRequest(const JSON::Object::Ptr object) :
	GWRequest(object)
{
}

void GWSetValueRequest::setDeviceID(const DeviceID &deviceID)
{
	json()->set("device_id", deviceID.toString());
}

DeviceID GWSetValueRequest::deviceID() const
{
	return DeviceID::parse(json()->getValue<string>("device_id"));
}

void GWSetValueRequest::setModuleID(const ModuleID &moduleID)
{
	json()->set("module_id", moduleID.value());
}

ModuleID GWSetValueRequest::moduleID() const
{
	return ModuleID::parse(json()->getValue<string>("module_id"));
}

void GWSetValueRequest::setValue(double value)
{
	if (std::isnan(value))
		throw InvalidArgumentException("value must not be NaN");
	else
		json()->set("value", value);
}

double GWSetValueRequest::value() const
{
	return json()->getValue<double>("value");
}

void GWSetValueRequest::setTimeout(const Timespan &timeout)
{
	json()->set("timeout", timeout.totalSeconds());
}

Timespan GWSetValueRequest::timeout() const
{
	return json()->getValue<unsigned>("timeout") * Timespan::SECONDS;
}

void GWSetValueRequest::setMode(const OpMode &mode)
{
	json()->set("mode", mode.toString());
}

OpMode GWSetValueRequest::mode() const
{
	if (!json()->has("mode"))
		return OpMode::TRY_ONCE;

	return OpMode::parse(json()->getValue<string>("mode"));
}
