#include <Poco/Exception.h>

#include "model/DeviceCriteria.h"

using namespace std;
using namespace BeeeOn;
using namespace Poco;
using namespace Poco::Net;

DeviceCriteria::DeviceCriteria(const IPAddress &address):
	m_ipAddress(address)
{
}

DeviceCriteria::DeviceCriteria(const MACAddress &address):
	m_macAddress(address)
{
}

DeviceCriteria::DeviceCriteria(const uint64_t serialNumber):
	m_serialNumber(serialNumber)
{
}

bool DeviceCriteria::hasIPAddress() const
{
	return !m_ipAddress.isNull();
}

IPAddress DeviceCriteria::ipAddress() const
{
	return m_ipAddress;
}

bool DeviceCriteria::hasMACAddress() const
{
	return !m_macAddress.isNull();
}

MACAddress DeviceCriteria::macAddress() const
{
	return m_macAddress;
}

bool DeviceCriteria::hasSerialNumber() const
{
	return !m_serialNumber.isNull();
}

uint64_t DeviceCriteria::serialNumber() const
{
	return m_serialNumber;
}

string DeviceCriteria::toString() const
{
	if (!m_ipAddress.isNull())
		return m_ipAddress.value().toString();
	if (!m_macAddress.isNull())
		return m_macAddress.value().toString();
	if (!m_serialNumber.isNull())
		return to_string(m_serialNumber.value());

	throw IllegalStateException("no search criteria");
}
