#pragma once

#include <functional>
#include <string>

namespace Poco {

class Thread;

}

namespace BeeeOn {

/**
 * @brief The class implements working with POSIX signals.
 */
class PosixSignal {
private:
	PosixSignal();

public:
	typedef std::function<void(unsigned int)> Handler;

	/**
	 * Send signal to a process of the given pid.
	 * @param pid Process ID
	 * @param name Name of the signal to send (e.g. SIGUSR1)
	 */
	static void send(long pid, const std::string name);

	/**
	 * Send signal to a thread owning by this process. We assume that the Poco::Thread
	 * is using pthread internally. The implementation calls pthread_kill() to send
	 * the signal to the proper thread.
	 *
	 * @param thread Thread to send the signal to
	 * @param name Name of the signal to send
	 */
	static void send(const Poco::Thread &thread, const std::string &name);

	/**
	 * Ignore the selected signal to not influence the current process (SIG_IGN).
	 * @param name Name of the signal to be ignored
	 */
	static void ignore(const std::string &name);

	/**
	 * Install a handler for the given signal name.
	 * @param name Name of the signal to handle
	 * @param handler Handler to be used
	 */
	static void handle(const std::string &name, Handler handler);

	/**
	 * Install appropriate handlers for the most fatal signals like
	 * SIGSEGV, SIGBUS, etc. The application would provide some more
	 * information during the actual crash. The handlers terminates
	 * the application by calling _exit().
	 */
	static void trapFatal();

protected:
	/**
	 * Translate name of a signal to its internal representation.
	 */
	static unsigned int byName(const std::string &name);

	static void send(long pid, unsigned int num);
	static void send(const Poco::Thread &thread, unsigned int num);
	static void ignore(unsigned int num);
	static void handle(unsigned int num, Handler handler);
};

}
