#pragma once

namespace BeeeOn {

/**
 * @brief Representation of a logical expression in form:
 * <code>left</code> <em>operator</em> <code>right</code>.
 * The operator can be on of:
 *
 * - == - equals
 * - != - not equals
 *
 * The <code>left</code> and <code>right</code> parts are
 * strings without whitespace.
 */
class LogicalExpression {
public:
	enum Op {
		OP_EQUAL,
		OP_NOT_EQUAL,
	};

	LogicalExpression(
		const std::string &left,
		const std::string &right,
		const Op op);

	/**
	 * @brief Evaluates the expression.
	 * @returns result of the expression
	 */
	bool result() const;

	std::string left() const;
	std::string right() const;
	Op op() const;

	static LogicalExpression parse(const std::string &input);

private:
	bool computeEqual() const;

private:
	const std::string m_left;
	const std::string m_right;
	const Op m_op;
};

}
