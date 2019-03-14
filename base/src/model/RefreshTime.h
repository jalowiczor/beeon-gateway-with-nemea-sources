#pragma once

#include <iosfwd>
#include <string>

#include <Poco/Timespan.h>

namespace BeeeOn {

/**
 * @brief RefreshTime represents time of refreshing values from sensors.
 * E.g. sensors are polled periodically once per refresh time or sensors
 * reports their values periodically once per refresh time.
 *
 * However, sensors can be some times set to a different mode where values
 * are not reported periodically but the reports are generated based changes
 * (event based). Some sensors does not support refresh time and always
 * works this way.
 *
 * The class RefreshTime models the situation described above. The refresh
 * time can be NONE (unsupported by device), DISABLED (reporting is event
 * based), or a number of seconds.
 */
class RefreshTime {
public:
	static const RefreshTime NONE;
	static const RefreshTime DISABLED;

	RefreshTime();

private:
	RefreshTime(const Poco::Timespan &time);

public:
	bool isDisabled() const;
	bool isNone() const;

	Poco::Timespan time() const;
	int seconds() const;

	operator Poco::Timespan() const
	{
		return time();
	}

	bool operator ==(const RefreshTime &other) const;
	bool operator !=(const RefreshTime &other) const;
	bool operator <(const RefreshTime &other) const;
	bool operator <=(const RefreshTime &other) const;
	bool operator >(const RefreshTime &other) const;
	bool operator >=(const RefreshTime &other) const;

	std::string toString() const;
	static RefreshTime fromSeconds(const int seconds);
	static RefreshTime fromMinutes(const int minutes);
	static RefreshTime parse(const std::string &input);

private:
	Poco::Timespan m_time;
};

inline std::ostream &operator <<(std::ostream &s, const RefreshTime &time)
{
	return s << time.toString();
}

}
