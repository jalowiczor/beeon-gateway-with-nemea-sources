#include <Poco/Exception.h>
#include <Poco/NumberParser.h>
#include <Poco/StringTokenizer.h>

#include "util/TimespanParser.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

bool TimespanParser::tryParseUnit(const string &input, Timespan::TimeDiff &multiplier)
{
	if (input == "d")
		multiplier = Timespan::DAYS;
	else if (input == "h")
		multiplier = Timespan::HOURS;
	else if (input == "m")
		multiplier = Timespan::MINUTES;
	else if (input == "s")
		multiplier = Timespan::SECONDS;
	else if (input == "ms")
		multiplier = Timespan::MILLISECONDS;
	else if (input == "us" || input.empty())
		multiplier = 1;
	else
		return false;

	return true;
}

bool TimespanParser::tryParse(const string &input, Timespan &timespan)
{
	StringTokenizer toks(input, " \t",
		StringTokenizer::TOK_TRIM | StringTokenizer::TOK_IGNORE_EMPTY);

	if (toks.count() != 1 && toks.count() != 2)
		return false;

	int64_t span;
	if (!NumberParser::tryParse64(toks[0], span))
		return false;

	if (toks.count() == 1) {
		timespan = span;
		return true;
	}

	Timespan::TimeDiff multiplier = 0;
	if (!tryParseUnit(toks[1], multiplier))
		return false;

	timespan = span * multiplier;
	return true;
}

Timespan TimespanParser::parse(const string &input)
{
	StringTokenizer toks(input, " \t",
		StringTokenizer::TOK_TRIM | StringTokenizer::TOK_IGNORE_EMPTY);

	if (toks.count() == 1)
		return NumberParser::parse64(toks[0]);

	if (toks.count() == 2) {
		Timespan::TimeDiff multiplier = 0;
		if (!tryParseUnit(toks[1], multiplier))
			throw SyntaxException("invalid timespan unit: " + toks[1].substr(0, 5));

		return NumberParser::parse64(toks[0]) * multiplier;

	}

	throw SyntaxException("unexpected timespan input");
}
