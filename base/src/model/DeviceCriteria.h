#pragma once

#include <string>

#include <Poco/Nullable.h>
#include <Poco/Net/IPAddress.h>

#include "net/MACAddress.h"

namespace BeeeOn {

/**
 * @brief DeviceCriteria holds a criteria for device searching.
 * Device can be search by one of the following properties:
 *
 * - by IP address
 * - by MAC address
 * - by serial number
 */
class DeviceCriteria {
public:
	DeviceCriteria(const Poco::Net::IPAddress &address);
	DeviceCriteria(const MACAddress &address);
	DeviceCriteria(const uint64_t serialNumber);

	bool hasIPAddress() const;
	Poco::Net::IPAddress ipAddress() const;

	bool hasMACAddress() const;
	MACAddress macAddress() const;

	bool hasSerialNumber() const;
	uint64_t serialNumber() const;

	std::string toString() const;

private:
	Poco::Nullable<Poco::Net::IPAddress> m_ipAddress;
	Poco::Nullable<MACAddress> m_macAddress;
	Poco::Nullable<uint64_t> m_serialNumber;
};

}
