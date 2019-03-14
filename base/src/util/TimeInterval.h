#pragma once

#include <Poco/Timespan.h>
#include <Poco/Timestamp.h>

namespace BeeeOn {

/**
 * Interval between two timestamps. The start must always be
 * less or equal to end. The duration of the interval is
 * defined as:
 *
 *    m_end - m_start
 *
 * The m_end is not part of the interval.
 */
class TimeInterval {
public:
	TimeInterval(const Poco::Timestamp &start,
			const Poco::Timestamp &end);
	~TimeInterval();

	/**
	 * Create interval for past range from the given end reference
	 * timepoint. Examples:
	 *
	 * <pre>
	 * TimeInterval last5Min = TimeInterval::past(5 * Timespan::MINUTES);
	 * TimeInterval lastHourOfYear = TimeInterval::past(
	 * 		1 * TimeInterval::HOURS,
	 * 		Timestamp::fromEpochTime(1483225200));
	 * <pre>
	 *
	 * The last5Min is interval lasting 5 minutes just before now.
	 * The lastHourOfYear is an interval representing the last hour of year 2016.
	 */
	static TimeInterval past(const Poco::Timespan &range,
			const Poco::Timestamp &end = Poco::Timestamp());

	/**
	 * The time interval is empty (start == end).
	 */
	bool isEmpty() const;

	/**
	 * The time interval both starts and ends before
	 * the given timestampt at.
	 */
	bool isBefore(const Poco::Timestamp &at) const;

	Poco::Timestamp start() const;
	Poco::Timestamp end() const;

	const Poco::Timestamp &start();
	const Poco::Timestamp &end();

private:
	Poco::Timestamp m_start;
	Poco::Timestamp m_end;
};

}
