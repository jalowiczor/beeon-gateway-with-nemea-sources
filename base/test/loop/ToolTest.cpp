#include <sstream>

#include <cppunit/extensions/HelperMacros.h>

#include <Poco/Event.h>

#include "cppunit/BetterAssert.h"
#include "io/StreamConsole.h"
#include "loop/Tool.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class ToolTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(ToolTest);
	CPPUNIT_TEST(testStartNothing);
	CPPUNIT_TEST(testStartWithArgs);
	CPPUNIT_TEST(testInteractiveHelp);
	CPPUNIT_TEST(testInteractiveArgs);
	CPPUNIT_TEST(testInteractiveRepeat);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();

	void testStartNothing();
	void testStartWithArgs();
	void testInteractiveHelp();
	void testInteractiveArgs();
	void testInteractiveRepeat();

private:
	istringstream m_in;
	ostringstream m_out;
	StreamConsole::Ptr m_console;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ToolTest);

class TestableTool : public Tool {
public:
	TestableTool(): Tool(false, false)
	{
	}

protected:
	void main(
		ConsoleSession &,
		const vector<string> &args) override
	{
		m_args = args;
		m_done.set();
	}

public:
	std::vector<string> m_args;
	Poco::Event m_done;
};

class TestableInteractiveTool : public Tool {
public:
	TestableInteractiveTool(): Tool(false, true)
	{
	}

	void notify()
	{
		stopControl().requestWakeup();
	}

protected:
	void main(
		ConsoleSession &session,
		const vector<string> &args) override
	{
		string line = session.readLine();
		if (line == "help") {
			session.print("help interactive");
		}
		else if (line == "args") {
			for (const auto &arg : args)
				session.print(arg);
		}

		m_done.set();
		stopControl().waitStoppable(-1);
	}

public:
	Poco::Event m_done;
};

void ToolTest::setUp()
{
	m_in.str("");
	m_out.str("");
	m_console = new StreamConsole(m_in, m_out);
}

void ToolTest::testStartNothing()
{
	TestableTool tool;
	tool.setConsole(m_console);

	tool.start();
	CPPUNIT_ASSERT_NO_THROW(tool.m_done.wait(10000));
	tool.stop();

	CPPUNIT_ASSERT(tool.m_args.empty());
}

void ToolTest::testStartWithArgs()
{
	TestableTool tool;
	tool.setConsole(m_console);
	tool.setCommand("list -s -x -y");

	tool.start();
	CPPUNIT_ASSERT_NO_THROW(tool.m_done.wait(10000));
	tool.stop();

	CPPUNIT_ASSERT_EQUAL(4, tool.m_args.size());
	CPPUNIT_ASSERT_EQUAL("list", tool.m_args[0]);
	CPPUNIT_ASSERT_EQUAL("-s", tool.m_args[1]);
	CPPUNIT_ASSERT_EQUAL("-x", tool.m_args[2]);
	CPPUNIT_ASSERT_EQUAL("-y", tool.m_args[3]);
}

void ToolTest::testInteractiveHelp()
{
	TestableInteractiveTool tool;
	tool.setConsole(m_console);
	m_in.str("help\n");

	tool.start();
	CPPUNIT_ASSERT_NO_THROW(tool.m_done.wait(10000));
	tool.stop();

	CPPUNIT_ASSERT_EQUAL("> help interactive\n", m_out.str());
}

void ToolTest::testInteractiveArgs()
{
	TestableInteractiveTool tool;
	tool.setConsole(m_console);
	tool.setCommand("1 2 -x -5");
	m_in.str("args\n");

	tool.start();
	CPPUNIT_ASSERT_NO_THROW(tool.m_done.wait(10000));
	tool.stop();

	CPPUNIT_ASSERT_EQUAL("> 1\n2\n-x\n-5\n", m_out.str());
}

void ToolTest::testInteractiveRepeat()
{
	TestableInteractiveTool tool;
	tool.setConsole(m_console);
	tool.setCommand("1 2 -x -5");

	m_in.str("args\n");
	tool.start();
	CPPUNIT_ASSERT_NO_THROW(tool.m_done.wait(10000));
	CPPUNIT_ASSERT_EQUAL("> 1\n2\n-x\n-5\n", m_out.str());

	m_out.str("");

	m_in.str("help\n");
	tool.notify();
	CPPUNIT_ASSERT_NO_THROW(tool.m_done.wait(10000));
	CPPUNIT_ASSERT_EQUAL("> help interactive\n", m_out.str());

	m_out.str("");

	m_in.str("args\n");
	tool.notify();
	CPPUNIT_ASSERT_NO_THROW(tool.m_done.wait(10000));
	CPPUNIT_ASSERT_EQUAL("> 1\n2\n-x\n-5\n", m_out.str());

	m_out.str("");

	tool.stop();

	CPPUNIT_ASSERT_EQUAL("", m_out.str());
}

}
