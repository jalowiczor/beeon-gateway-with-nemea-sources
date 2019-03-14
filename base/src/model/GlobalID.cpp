#include "model/GlobalID.h"

using namespace std;
using namespace BeeeOn;
using namespace Poco;

/**
 * Implementation taken from
 *
 *  http://developer.classpath.org/doc/java/util/UUID-source.html
 */
unsigned int GlobalID::hash() const
{
	union {
		char b[BYTES_SIZE];
		uint32_t u32[WORDS_SIZE];
	};
	m_uuid.copyTo(b);

	return (u32[0] ^ u32[1]) ^ (u32[2] ^ u32[3]);
}

GlobalID GlobalID::fromBytes(const vector<uint8_t> &in)
{
	if (in.size() < BYTES_SIZE)
		throw InvalidArgumentException("invalid count " + to_string(in.size()) + " of bytes for UUID");

	UUID uuid;
	uuid.copyFrom(reinterpret_cast<const char *>(in.data()));

	return GlobalID(uuid);
}

vector<uint8_t> GlobalID::toBytes() const
{
	vector<uint8_t> buff(BYTES_SIZE);
	m_uuid.copyTo(reinterpret_cast<char *>(buff.data()));

	return buff;
}
