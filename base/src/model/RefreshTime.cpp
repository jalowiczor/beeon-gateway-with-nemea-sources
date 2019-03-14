#include <Poco/Exception.h>
#include <Poco/NumberParser.h>

#include "model/RefreshTime.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

const RefreshTime RefreshTime::NONE = RefreshTime(-1);
const RefreshTime RefreshTime::DISABLED = RefreshTime(0);

RefreshTime::RefreshTime():
	m_time(-1)
{
}

RefreshTime::RefreshTime(const Timespan &time):
	m_time(time)
{
	poco_assert_msg(m_time <= 0 || time >= 1 * Timespan::SECONDS,
		"refresh time must be at least 1 second");

	poco_assert_msg(m_time >= -1,
		"negative refresh time must always be -1");
}

bool RefreshTime::isDisabled() const
{
	return m_time == 0;
}

bool RefreshTime::isNone() const
{
	return m_time < 0;
}

Timespan RefreshTime::time() const
{
	return m_time;
}

int RefreshTime::seconds() const
{
	if (m_time < 0)
		return -1;

	return m_time.totalSeconds();
}

bool RefreshTime::operator ==(const RefreshTime &other) const
{
	return m_time == other.m_time;
}

bool RefreshTime::operator !=(const RefreshTime &other) const
{
	return m_time != other.m_time;
}

bool RefreshTime::operator <(const RefreshTime &other) const
{
	return m_time < other.m_time;
}

bool RefreshTime::operator <=(const RefreshTime &other) const
{
	return m_time <= other.m_time;
}

bool RefreshTime::operator >(const RefreshTime &other) const
{
	return m_time > other.m_time;
}

bool RefreshTime::operator >=(const RefreshTime &other) const
{
	return m_time >= other.m_time;
}

string RefreshTime::toString() const
{
	if (isDisabled())
		return "disabled";
	if (isNone())
		return "none";

	return to_string(m_time.totalSeconds());
}

RefreshTime RefreshTime::fromSeconds(const int seconds)
{
	if (seconds == 0)
		return DISABLED;

	if (seconds < 0)
		return NONE;

	return RefreshTime(seconds * Timespan::SECONDS);
}

RefreshTime RefreshTime::fromMinutes(const int minutes)
{
	if (minutes == 0)
		return DISABLED;

	if (minutes < 0)
		return NONE;

	return RefreshTime(minutes * Timespan::MINUTES);
}

RefreshTime RefreshTime::parse(const string &input)
{
	if (input == "disabled")
		return DISABLED;

	if (input == "none")
		return NONE;

	const int time = NumberParser::parse(input);
	return fromSeconds(time);
}
