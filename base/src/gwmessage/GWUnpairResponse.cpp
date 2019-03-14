#include "gwmessage/GWUnpairResponse.h"

using namespace std;
using namespace Poco;
using namespace Poco::JSON;
using namespace BeeeOn;

GWUnpairResponse::GWUnpairResponse():
	GWResponse(GWMessageType::UNPAIR_RESPONSE)
{
}

GWUnpairResponse::GWUnpairResponse(const Object::Ptr object):
	GWResponse(object)
{
}

void GWUnpairResponse::setUnpairedDevices(const set<DeviceID> &devices)
{
	Array::Ptr array = new Array;

	for (const auto &id : devices) {
		Object::Ptr entry = new Object;
		entry->set("device_id", id.toString());
		array->add(entry);
	}

	json()->set("devices", array);
}

set<DeviceID> GWUnpairResponse::unpairedDevices() const
{
	set<DeviceID> devices;
	Array::Ptr array = json()->getArray("devices");

	if (array.isNull())
		return devices;

	for (size_t i = 0; i < array->size(); ++i) {
		Object::Ptr entry = array->getObject(i);
		const string &id = entry->getValue<string>("device_id");
		devices.emplace(DeviceID::parse(id));
	}

	return devices;
}
