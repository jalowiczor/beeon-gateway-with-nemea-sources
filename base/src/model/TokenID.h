#pragma once

#include <string>

namespace BeeeOn {

/**
 * Class TokenID provides implementation of type encapsulating tokens
 * used in user authorisation processes.
 */
class TokenID {
public:
	TokenID();

	TokenID(const std::string &value)
	{
		m_value = value;
	}

	static TokenID parse(const std::string &value)
	{
		return TokenID(value);
	}

	bool isNull() const
	{
		return m_value.empty();
	}

	std::string toString() const
	{
		return m_value;
	}

	bool operator !=(const TokenID &id) const
	{
		return m_value != id.m_value;
	}

	bool operator ==(const TokenID &id) const
	{
		return m_value == id.m_value;
	}

	bool operator <(const TokenID &id) const
	{
		return m_value < id.m_value;
	}

	bool operator >(const TokenID &id) const
	{
		return m_value > id.m_value;
	}

	bool operator <=(const TokenID &id) const
	{
		return m_value <= id.m_value;
	}

	bool operator >=(const TokenID &id) const
	{
		return m_value >= id.m_value;
	}

private:
	std::string m_value;
};

}
