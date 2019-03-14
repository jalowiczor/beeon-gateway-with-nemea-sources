#include <Poco/Exception.h>

#include "model/CustomTypeID.h"

using namespace BeeeOn;
using namespace Poco;
using namespace std;

CustomTypeID::CustomTypeID()
{
}

CustomTypeID::CustomTypeID(const string &id):
	m_value(id)
{
}

string CustomTypeID::toString() const
{
	return m_value;
}

CustomTypeID CustomTypeID::parse(const string &input)
{
	if (input.length() > MAX_LENGTH)
		throw InvalidArgumentException("too long custom ID (" + to_string(input.length()) + ")");

	return CustomTypeID(input);
}

bool CustomTypeID::isNull() const
{
	return m_value.empty();
}

bool CustomTypeID::operator ==(const CustomTypeID &id) const
{
	return m_value == id.m_value;
}

bool CustomTypeID::operator !=(const CustomTypeID &id) const
{
	return m_value != id.m_value;
}

bool CustomTypeID::operator <(const CustomTypeID &id) const
{
	return m_value < id.m_value;
}

bool CustomTypeID::operator >(const CustomTypeID &id) const
{
	return m_value > id.m_value;
}

bool CustomTypeID::operator <=(const CustomTypeID &id) const
{
	return m_value <= id.m_value;
}

bool CustomTypeID::operator >=(const CustomTypeID &id) const
{
	return m_value >= id.m_value;
}
