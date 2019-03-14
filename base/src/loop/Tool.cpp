#include <Poco/Exception.h>
#include <Poco/Logger.h>
#include <Poco/Process.h>

#include "io/StdConsole.h"
#include "loop/Tool.h"
#include "util/ArgsParser.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

Tool::Tool(bool terminate, bool repeat):
	m_terminate(terminate),
	m_repeat(repeat),
	m_console(new StdConsole),
	m_runner(*this, &Tool::parseAndRun)
{
}

void Tool::setTerminate(bool terminate)
{
	m_terminate = terminate;
}

void Tool::setRepeat(bool repeat)
{
	m_repeat = repeat;
}

void Tool::setCommand(const string &command)
{
	m_command = command;
}

void Tool::setConsole(Console::Ptr console)
{
	m_console = console;
}

string Tool::command() const
{
	return m_command;
}

void Tool::start()
{
	m_thread.start(m_runner);
}

void Tool::stop()
{
	m_console->close();
	m_stopControl.requestStop();
	m_thread.join();
}

StopControl &Tool::stopControl()
{
	return m_stopControl;
}

void Tool::startSession(const vector<string> &args)
{
	ConsoleSession session(*m_console);

	try {
		main(session, args);
	}
	BEEEON_CATCH_CHAIN(logger())
}

void Tool::parseAndRun()
{
	StopControl::Run run(m_stopControl);
	ArgsParser parser;

	do {
		vector<string> args;
		try {
			args = parser.parse(m_command);
		}
		BEEEON_CATCH_CHAIN_ACTION(logger(), break)

		try {
			startSession(args);
		}
		BEEEON_CATCH_CHAIN_ACTION(logger(), break)
	} while (m_repeat && run);

	if (!m_terminate)
		return;
			
	const auto pid = Process::id();

	try {
		Process::requestTermination(pid);
	}
	catch (const NotFoundException &) {
		return; // our process does not exist?
	}
	BEEEON_CATCH_CHAIN(logger())
}

bool Tool::shouldStop() const
{
	return m_stopControl.shouldStop();
}
