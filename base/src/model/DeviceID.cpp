#include <exception>
#include <sstream>

#include <Poco/Exception.h>
#include <Poco/NumberParser.h>
#include <Poco/Random.h>

#include "model/DeviceID.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

const unsigned int DeviceID::IDENT_WIDTH = 56;
const uint64_t DeviceID::IDENT_MASK = (((uint64_t) 1) << IDENT_WIDTH) - 1;
const uint64_t DeviceID::PREFIX_MASK = ~IDENT_MASK;

DeviceID::DeviceID():
	m_value(0)
{
}

DeviceID::DeviceID(uint64_t value):
	m_value(value)
{
	const int shift = is32bit()? 24 : IDENT_WIDTH;

	if ((m_value >> shift) == 0) {
		throw InvalidArgumentException(
			"no prefix present in the given device ID: "
			+ to_string(m_value));
	}
}

DeviceID::DeviceID(const DevicePrefix &prefix, uint64_t ident)
{
	if (ident & PREFIX_MASK) {
		throw InvalidArgumentException(
			"ident part of device ID must not overflow 56 bits");
	}

	m_value = ((uint64_t) prefix) << IDENT_WIDTH;
	m_value |= ident;
}

DeviceID DeviceID::parse(const string &s)
{
	if (s.size() >= 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
		return NumberParser::parseHex64(s);

	return NumberParser::parseUnsigned64(s);
}

string DeviceID::toString() const
{
	ostringstream ss;

	ss << "0x" << std::hex;

	if (is32bit())
		ss << (uint32_t) m_value;
	else
		ss << m_value;

	return ss.str();
}

DeviceID DeviceID::random(const DevicePrefix &prefix)
{
	Random rnd;
	rnd.seed();

	uint64_t ident = rnd.next();
	ident <<= 24;
	ident |= rnd.next(1 << 24);

	return DeviceID(prefix, ident);
}
