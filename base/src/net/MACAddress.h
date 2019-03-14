#pragma once

#include <string>
#include <vector>

namespace BeeeOn {

class MACAddress {
public:
	MACAddress(const unsigned char bytes[6]);
	MACAddress(const std::vector<unsigned char> &bytes);

	MACAddress(const uint64_t numMac);

	MACAddress();

	uint64_t toNumber() const;

	std::string toString() const;

	std::string toString(const char separator) const;

	std::vector<unsigned char> bytes() const
	{
		return m_bytes;
	}

	/**
	 * Copy the raw MAC address bytes into the given array.
	 */
	void into(unsigned char bytes[6]) const;

	operator uint64_t() const
	{
		return toNumber();
	}

	bool operator ==(const MACAddress &mac) const
	{
		return (mac.bytes() == m_bytes);
	}

	bool operator !=(const MACAddress &mac) const
	{
		return (mac.bytes() != m_bytes);
	}

	bool operator <(const MACAddress &mac) const
	{
		return (toNumber() < mac.toNumber());
	}

	bool operator >(const MACAddress &mac) const
	{
		return (toNumber() > mac.toNumber());
	}

	static MACAddress parse(const std::string &str, const char separator = ':');

	static const MACAddress ZERO;

private:
	std::vector<unsigned char> m_bytes;
};

}
