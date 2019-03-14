#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Exception.h>

#include "cppunit/BetterAssert.h"
#include "math/LogicalExpression.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class LogicalExpressionTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(LogicalExpressionTest);
	CPPUNIT_TEST(testEqual);
	CPPUNIT_TEST(testNotEqual);
	CPPUNIT_TEST(testParseInvalid);
	CPPUNIT_TEST(testParse);
	CPPUNIT_TEST_SUITE_END();
public:
	void testEqual();
	void testNotEqual();
	void testParseInvalid();
	void testParse();
};

CPPUNIT_TEST_SUITE_REGISTRATION(LogicalExpressionTest);

void LogicalExpressionTest::testEqual()
{
	LogicalExpression t("text1", "text1", LogicalExpression::OP_EQUAL);
	LogicalExpression f("text1", "text2", LogicalExpression::OP_EQUAL);

	CPPUNIT_ASSERT(t.result());
	CPPUNIT_ASSERT(!f.result());
}

void LogicalExpressionTest::testNotEqual()
{
	LogicalExpression t("text1", "text2", LogicalExpression::OP_NOT_EQUAL);
	LogicalExpression f("text1", "text1", LogicalExpression::OP_NOT_EQUAL);

	CPPUNIT_ASSERT(t.result());
	CPPUNIT_ASSERT(!f.result());
}

void LogicalExpressionTest::testParseInvalid()
{
	CPPUNIT_ASSERT_THROW(
		LogicalExpression::parse("text1 text == text2"),
		SyntaxException);
	CPPUNIT_ASSERT_THROW(
		LogicalExpression::parse("text1 === text1"),
		SyntaxException);
	CPPUNIT_ASSERT_THROW(
		LogicalExpression::parse("text1"),
		SyntaxException);
	CPPUNIT_ASSERT_THROW(
		LogicalExpression::parse("text1 = text1"),
		SyntaxException);
	CPPUNIT_ASSERT_THROW(
		LogicalExpression::parse("text1 text1"),
		SyntaxException);
}

void LogicalExpressionTest::testParse()
{
	const auto e0 = LogicalExpression::parse("text1 == text1");
	CPPUNIT_ASSERT(e0.result());

	const auto e1 = LogicalExpression::parse("text1 == text2");
	CPPUNIT_ASSERT(!e1.result());

	const auto e2 = LogicalExpression::parse("text1 != text2");
	CPPUNIT_ASSERT(e2.result());

	const auto e3 = LogicalExpression::parse("text2 != text2");
	CPPUNIT_ASSERT(!e3.result());
}

}
