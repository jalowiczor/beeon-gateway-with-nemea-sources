#pragma once

#include <string>
#include <vector>

#include <Poco/AtomicCounter.h>
#include <Poco/Event.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/SharedPtr.h>
#include <Poco/Thread.h>

#include "io/Console.h"
#include "loop/StoppableLoop.h"
#include "loop/StopControl.h"
#include "util/Loggable.h"

namespace BeeeOn {
	
/**
 * @brief The purpose of the class Tool is to provide a common set
 * of features for loops that are to be used as command line tools.
 * We can reuse the dependency-injection infrastructure to start a
 * simple tool for some additional operation that is to be done out
 * of the scoped of the main application.
 *
 * Such a tool parses the command line and executes commands based
 * on the command line options. The command line is injected via
 * method setCommand(), usually we use dependency-injection for this
 * purpose.
 *
 * Any Tool implementation can be executed via the LoopRunner or
 * as a standalone loop. When running in a non-standalone mode, it
 * must be configured to not terminate its process on exit via
 * setTerminate(false). Otherwise, such loop would terminate all
 * other threads on exit.
 *
 * Each tool has a configurable Console implementation. The default
 * one is the StdConsole. Some Console implementations allows to
 * start the tool multiple times. Thus, opening a session leads to
 * a new execution of the tool's main. Each tool implementation should
 * be stateless at the class level.
 */
class Tool :
	public StoppableLoop,
	protected virtual Loggable {
public:
	typedef Poco::SharedPtr<Tool> Ptr;

	Tool(bool terminate = true, bool repeat = false);

	void setTerminate(bool terminate);
	void setRepeat(bool repeat);
	void setCommand(const std::string &command);
	void setConsole(Console::Ptr console);

	/**
	 * Start the tool in a separate thread. It parses the command
	 * and passes the args vector into the main method.
	 */
	void start() override;

	/**
	 * Request the tool thread to stop. This can be useful especially
	 * when it is doing some long running procedure (I/O). Otherwise,
	 * a tool usually exits quickly (unless configured repeatable).
	 */
	void stop() override;

protected:
	/**
	 * The main method of the tool that is executed in the Tool's loop thread
	 * with pre-parsed args. The original command line can be obtained by
	 * method command() if needed.
	 *
	 * All I/O should be done via the given ConsoleSession instance.
	 * Exceptions are cougth by the caller of the main method.
	 */
	virtual void main(
		ConsoleSession &session,
		const std::vector<std::string> &args) = 0;

	/**
	 * @return true if the tool should stop its processing
	 */
	bool shouldStop() const;

	/**
	 * Tool's loop thread entry point. It parses command line arguments,
	 * creates a new console session and execute the main method. If the
	 * main method fails, the session is closed and the tool can be terminated
	 * if the m_terminate property is set to true. Otherwise, the method
	 * would repeat again waiting for a new session.
	 */
	void parseAndRun();

	/**
	 * Start a console session and execute main.
	 * Exceptions thrown from main are not propagated.
	 */
	void startSession(const std::vector<std::string> &args);

	/**
	 * @return original command line
	 */
	std::string command() const;

	/**
	 * @returns underlying StopControl instance.
	 */
	StopControl &stopControl();

private:
	bool m_terminate;
	bool m_repeat;
	std::string m_command;
	Console::Ptr m_console;
	StopControl m_stopControl;
	Poco::RunnableAdapter<Tool> m_runner;
	Poco::Thread m_thread;
};

}
