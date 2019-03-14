#include <string>

#include "gwmessage/GWSensorDataExport.h"
#include "util/JsonUtil.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

GWSensorDataExport::GWSensorDataExport():
	GWMessage(GWMessageType::SENSOR_DATA_EXPORT)
{
}

GWSensorDataExport::GWSensorDataExport(const JSON::Object::Ptr object):
	GWMessage(object)
{
}

GWSensorDataConfirm::Ptr GWSensorDataExport::confirm() const
{
	GWSensorDataConfirm::Ptr confirm(new GWSensorDataConfirm);
	confirm->setID(id());

	return confirm;
}

void GWSensorDataExport::setData(const vector<SensorData> &data)
{
	JSON::Array::Ptr dataArray(new JSON::Array);
	for (const auto &dataItem : data) {
		JSON::Object::Ptr dataObject(new JSON::Object);

		dataObject->set("device_id", dataItem.deviceID().toString());
		dataObject->set("timestamp",
			dataItem.timestamp().value().epochMicroseconds());

		JSON::Array::Ptr valueArray(new JSON::Array);
		for (const auto &value : dataItem) {
			JSON::Object::Ptr valueObject(new JSON::Object);

			valueObject->set("module_id", value.moduleID().toString());
			valueObject->set("valid", value.isValid());

			if (value.isValid())
				valueObject->set("value", value.value());

			valueArray->add(Dynamic::Var(valueObject));
		}

		dataObject->set("values", valueArray);

		dataArray->add(Dynamic::Var(dataObject));
	}

	json()->set("data", dataArray);
}

vector<SensorData> GWSensorDataExport::data() const
{
	vector<SensorData> data;

	JSON::Array::Ptr dataArray = json()->getArray("data");
	for (size_t i = 0; i < dataArray->size(); i++) {
		const JSON::Object::Ptr dataObject = dataArray->getObject(i);

		SensorData dataItem;
		dataItem.setDeviceID(DeviceID::parse(
			dataObject->getValue<string>("device_id")));
		dataItem.setTimestamp(Timestamp(
			dataObject->getValue<uint64_t>("timestamp")));

		JSON::Array::Ptr valueArray = dataObject->getArray("values");
		for (size_t j = 0; j < valueArray->size(); j++) {
			const JSON::Object::Ptr valueObject = valueArray->getObject(j);

			SensorValue value;
			value.setModuleID(ModuleID::parse(
				valueObject->getValue<string>("module_id")));
			value.setValid(valueObject->getValue<bool>("valid"));

			if (value.isValid())
				value.setValue(valueObject->getValue<double>("value"));

			dataItem.insertValue(value);
		}

		data.push_back(dataItem);
	}

	return data;
}
