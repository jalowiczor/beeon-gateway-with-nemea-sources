#pragma once

#include <vector>

#include <Poco/Timestamp.h>

#include "model/DeviceID.h"
#include "model/ModuleID.h"
#include "model/SensorValue.h"
#include "util/IncompleteTimestamp.h"

namespace BeeeOn {

/*
 * Representation of data coming from a sensor. Every SensorData comes
 * from a single device identified by DeviceID. The SensorData holds
 * a list of measured values.Each of measured values is defined by
 * SensorValue instance.
 */
class SensorData {
public:
	SensorData();
	SensorData(
		const DeviceID &id,
		const Poco::Timestamp &timestamp,
		const std::vector<SensorValue> &values);

	void setDeviceID(const DeviceID &deviceID);
	const DeviceID deviceID() const;

	void setTimestamp(const IncompleteTimestamp &timestamp);
	const IncompleteTimestamp timestamp() const;

	bool isEmpty() const;

	void insertValue(const SensorValue &value);

	size_t size() const;

	SensorValue& at(size_t index);
	const SensorValue& at(size_t index) const;

	std::vector<SensorValue>::iterator begin();
	std::vector<SensorValue>::iterator end();

	std::vector<SensorValue>::const_iterator begin() const;
	std::vector<SensorValue>::const_iterator end() const;

	bool operator !=(const SensorData &data) const;
	bool operator ==(const SensorData &data) const;

	SensorValue& operator [](size_t index);
	const SensorValue& operator [](size_t index) const;

private:
	DeviceID m_deviceID;
	IncompleteTimestamp m_timestamp;
	std::vector<SensorValue> m_values;
};

}
