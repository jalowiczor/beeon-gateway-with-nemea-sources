#include <Poco/RegularExpression.h>
#include <Poco/TextConverter.h>
#include <Poco/UTF8Encoding.h>

#include "model/DeviceDescription.h"

using namespace BeeeOn;
using namespace Poco;
using namespace Poco::Net;
using namespace std;

static const RegularExpression NAME_PATTERN(
	"[^\\p{L}\\p{Nd} \\.:!?()/,\\-_#''$€¥£©®+]",
	RegularExpression::RE_UTF8
);

DeviceDescription::Builder::Builder()
{
}

DeviceDescription::Builder &DeviceDescription::Builder::id(const DeviceID &id)
{
	m_id = id;
	return *this;
}

DeviceDescription::Builder &DeviceDescription::Builder::type(
		const string &vendor,
		const string &name)
{
	m_vendor = vendor;
	m_product = name;
	return *this;
}

DeviceDescription::Builder &DeviceDescription::Builder::refreshTime(
		const Timespan &time)
{
	if (time >= 1 * Timespan::SECONDS) {
		m_refreshTime = RefreshTime::fromSeconds(time.totalSeconds());
	}
	else if (time == 0) {
		m_refreshTime = RefreshTime::DISABLED;
	}
	else if (time < 0) {
		m_refreshTime = RefreshTime::NONE;
	}
	else {
		m_refreshTime = RefreshTime::fromSeconds(1);
	}

	return *this;
}

DeviceDescription::Builder &DeviceDescription::Builder::disabledRefreshTime()
{
	m_refreshTime = RefreshTime::DISABLED;
	return *this;
}

DeviceDescription::Builder &DeviceDescription::Builder::noRefreshTime()
{
	m_refreshTime = RefreshTime::NONE;
	return *this;
}

DeviceDescription::Builder &DeviceDescription::Builder::name(
		const string &name)
{
	m_name = name;
	return *this;
}

DeviceDescription::Builder &DeviceDescription::Builder::firmware(
		const string &firmware)
{
	m_firmware = firmware;
	return *this;
}

DeviceDescription::Builder &DeviceDescription::Builder::ipAddress(
		const IPAddress &address)
{
	m_ipAddress = address;
	return *this;
}

DeviceDescription::Builder &DeviceDescription::Builder::macAddress(
		const MACAddress &mac)
{
	m_macAddress = mac;
	return *this;
}

DeviceDescription::Builder &DeviceDescription::Builder::serialNumber(
		const uint64_t serial)
{
	m_serialNumber = serial;
	return *this;
}

template <typename T>
static T notNull(const Nullable<T> value, const string &label)
{
	if (value.isNull())
		throw InvalidArgumentException(label + " was not set in builder");

	return value.value();
}

DeviceDescription DeviceDescription::Builder::build() const
{
	DeviceDescription description;
	description.setID(notNull(m_id, "device ID"));
	description.setVendor(notNull(m_vendor, "vendor name"));
	description.setProductName(notNull(m_product, "product name"));
	description.setDataTypes(m_modules);
	description.setRefreshTime(m_refreshTime);
	description.setFirmware(m_firmware);
	description.setName(m_name);

	if (!m_ipAddress.isNull())
		description.setIPAddress(m_ipAddress);
	if (!m_macAddress.isNull())
		description.setMACAddress(m_macAddress);
	if (!m_serialNumber.isNull())
		description.setSerialNumber(m_serialNumber);

	return description;
}

DeviceDescription::DeviceDescription()
{
}

void DeviceDescription::setID(const DeviceID &id)
{
	m_deviceID = id;
}

DeviceID DeviceDescription::id() const
{
	return m_deviceID;
}

void DeviceDescription::setVendor(const string &vendor)
{
	m_vendor = normalizeName(vendor);
}

string DeviceDescription::vendor() const
{
	return m_vendor;
}

void DeviceDescription::setProductName(const string &name)
{
	m_productName = normalizeName(name);
}

string DeviceDescription::productName() const
{
	return m_productName;
}

void DeviceDescription::setDataTypes(const list<ModuleType> &types)
{
	m_dataTypes = types;
}

list<ModuleType> DeviceDescription::dataTypes() const
{
	return m_dataTypes;
}

void DeviceDescription::setRefreshTime(const RefreshTime &time)
{
	m_refreshTime = time;
}

RefreshTime DeviceDescription::refreshTime() const
{
	return m_refreshTime;
}

void DeviceDescription::setName(const string &name)
{
	m_name = normalizeName(name);
}

string DeviceDescription::name() const
{
	return m_name;
}

void DeviceDescription::setFirmware(const string &firmware)
{
	m_firmware = normalizeName(firmware);
}

string DeviceDescription::firmware() const
{
	return m_firmware;
}

void DeviceDescription::setIPAddress(const IPAddress &ipAddress)
{
	m_ipAddress = ipAddress;
}

Nullable<IPAddress> DeviceDescription::ipAddress() const
{

	return m_ipAddress;
}

void DeviceDescription::setMACAddress(const MACAddress &macAddress)
{
	m_macAddress = macAddress;
}

Nullable<MACAddress> DeviceDescription::macAddress() const
{
	return m_macAddress;
}

void DeviceDescription::setSerialNumber(uint64_t serial)
{
	m_serialNumber = serial;
}

Nullable<uint64_t> DeviceDescription::serialNumber() const
{
	return m_serialNumber;
}

string DeviceDescription::toString() const
{
	string result;
	string modules;

	for (auto it = m_dataTypes.begin(); it != m_dataTypes.end(); ++it) {
		modules += it->type().toString();

		const auto &attributes = it->attributes();
		if (!attributes.empty())
			modules += ",";

		for (auto attr = attributes.begin(); attr != attributes.end(); ++attr) {
			modules += attr->toString();

			if (attr != --attributes.end())
				modules += ",";
		}

		if (it != --m_dataTypes.end())
			modules += " ";
	}

	result += m_deviceID.toString() + " ";
	result += m_vendor + " ";
	result += m_productName + " ";
	result += m_refreshTime.toString() + " ";
	result += modules + " ";

	return result;
}

string DeviceDescription::toPrettyString() const
{
	string summary = m_deviceID.toString() + " : " + m_vendor + " " + m_productName;
	summary.append("\n * RT : " + m_refreshTime.toString() + " s");

	size_t j = 0;
	for (const auto &m : m_dataTypes) {
		summary.append("\n * " + to_string(j++) + " : " + m.type().toString());

		for (const auto &a : m.attributes())
			summary.append("," + a.toString());
	}

	return summary;
}

string DeviceDescription::normalizeName(const string& bytes)
{
	const UTF8Encoding utf8;
	TextConverter text(utf8, utf8);
	string result;

	text.convert(bytes, result);
	NAME_PATTERN.subst(result, "?", RegularExpression::RE_GLOBAL);

	return result;
}
