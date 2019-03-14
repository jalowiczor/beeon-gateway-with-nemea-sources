#pragma once

#include <string>

#include <Poco/Timespan.h>

namespace BeeeOn {

class TimespanParser {
public:
	/**
	 * Parse the time unit: h (hour), d (day), m (minute), s (second), ms (millisecond), us (microsecond).
	 * The result is the multiplier to obtain microseconds. E.g. <code>tryParseUnit("s", m)</code> leads
	 * to <code>m == 1000000</code> (the number of microseconds in a second).
	 *
	 * When no explicit unit is given, the assumed unit is microseconds.
	 *
	 * @returns true if the parsing has been successful
	 */
	static bool tryParseUnit(const std::string &input, Poco::Timespan::TimeDiff &multiplier);

	/**
	 * Parse the given value expecting it to be a timespan specification. Example inputs:
	 * - _10 s_ (10 seconds)
	 * - _5 h_ (5 hours)
	 * - _128 us_ (128 microseconds)
	 *
	 * @see TimespanParser::tryParseUnit()
	 * @returns true if the parsing has been successful
	 */
	static bool tryParse(const std::string &input, Poco::Timespan &timespan);

	/**
	 * Parse the given value expecting it to be a timespan specification. In case of
	 * an invalid input, it throws a SyntaxException.
	 * @throws Poco::SyntaxException
	 */
	static Poco::Timespan parse(const std::string &input);
};

}
