#pragma once

#include <vector>

#include <Poco/UUID.h>
#include <Poco/UUIDGenerator.h>

namespace BeeeOn {

class GlobalID {
public:
	enum {
		BYTES_SIZE = 16,
		WORDS_SIZE = 4,
	};

	struct Hash {
		unsigned int operator() (const GlobalID &id)
		{
			return id.hash();
		}
	};

	GlobalID()
	{
	}

	GlobalID(const Poco::UUID &uuid):
		m_uuid(uuid)
	{
	}

	GlobalID(const GlobalID &gid):
		m_uuid(gid.m_uuid)
	{
	}

	bool isNull() const
	{
		return m_uuid.isNull();
	}

	static GlobalID random()
	{
		return GlobalID(Poco::UUIDGenerator::defaultGenerator()
				.createRandom());
	}

	static GlobalID parse(const std::string &s)
	{
		GlobalID gid;
		gid.m_uuid.parse(s);
		return gid;
	}

	static GlobalID fromBytes(const std::vector<uint8_t> &in);
	std::vector<uint8_t> toBytes() const;

	unsigned int hash() const;

	std::string toString() const
	{
		return m_uuid.toString();
	}

	bool operator !=(const GlobalID &id) const
	{
		return m_uuid != id.m_uuid;
	}

	bool operator ==(const GlobalID &id) const
	{
		return m_uuid == id.m_uuid;
	}

	bool operator <(const GlobalID &id) const
	{
		return m_uuid < id.m_uuid;
	}

	bool operator >(const GlobalID &id) const
	{
		return m_uuid > id.m_uuid;
	}

	bool operator <=(const GlobalID &id) const
	{
		return m_uuid <= id.m_uuid;
	}

	bool operator >=(const GlobalID &id) const
	{
		return m_uuid >= id.m_uuid;
	}

private:
	Poco::UUID m_uuid;
};

}
