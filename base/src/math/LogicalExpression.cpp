#include <Poco/Exception.h>
#include <Poco/RegularExpression.h>

#include "math/LogicalExpression.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

LogicalExpression::LogicalExpression(
		const string &left,
		const string &right,
		const Op op):
	m_left(left),
	m_right(right),
	m_op(op)
{
	switch (op) {
	case OP_EQUAL:
	case OP_NOT_EQUAL:
		return;
	}

	throw InvalidArgumentException("unknown operation " + to_string(op));
}

bool LogicalExpression::result() const
{
	switch (m_op) {
	case OP_EQUAL:
		return computeEqual();

	case OP_NOT_EQUAL:
		return !computeEqual();
	}

	throw IllegalStateException("illegal op: " + to_string(m_op));
}

bool LogicalExpression::computeEqual() const
{
	return m_left == m_right;
}

string LogicalExpression::left() const
{
	return m_left;
}

string LogicalExpression::right() const
{
	return m_right;
}

LogicalExpression::Op LogicalExpression::op() const
{
	return m_op;
}

LogicalExpression LogicalExpression::parse(const string &input)
{
	static const RegularExpression re("^(\\w+)\\s*(==|!=)\\s*(\\w+)$");

	RegularExpression::MatchVec m;
	if (!re.match(input, 0, m))
		throw SyntaxException("unrecognized logical expression: '" + input + "'");

	const auto &left = input.substr(m[1].offset, m[1].length);
	const auto &op = input.substr(m[2].offset, m[2].length);
	const auto &right = input.substr(m[3].offset, m[3].length);

	if (op == "==")
		return LogicalExpression(left, right, OP_EQUAL);
	if (op == "!=")
		return LogicalExpression(left, right, OP_NOT_EQUAL);

	throw IllegalStateException("unrecognized op: " + op);
}
