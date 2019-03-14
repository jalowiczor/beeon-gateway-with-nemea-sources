#pragma once

#include <iosfwd>

#include <Poco/AtomicCounter.h>
#include <Poco/Semaphore.h>
#include <Poco/SharedPtr.h>

#include "io/Console.h"

namespace BeeeOn {

class StreamConsoleSessionImpl : public ConsoleSessionImpl {
public:
	typedef Poco::SharedPtr<StreamConsoleSessionImpl> Ptr;

	StreamConsoleSessionImpl(
		Poco::Semaphore &semaphore,
		std::istream &in,
		std::ostream &out);
	~StreamConsoleSessionImpl();

	std::string readUntil(const char c) override;
	std::string readBytes(const unsigned int length) override;
	void print(const std::string &text, bool newline = true) override;
	bool eof() override;

private:
	Poco::Semaphore &m_semaphore;
	std::istream &m_in;
	std::ostream &m_out;
};

class StreamConsole : public Console {
public:
	typedef Poco::SharedPtr<StreamConsole> Ptr;

	StreamConsole(std::istream &in, std::ostream &out);

	void close() override;

protected:
	/**
	 * Waits until the semaphore is ready and then it
	 * creates a new instance of the StreamConsoleSessionImpl.
	 * The semaphore is set by the StreamConsoleSessionImpl's
	 * destructor to make another session available again.
	 */
	ConsoleSessionImpl::Ptr openSession() override;

private:
	/**
	 * There can be only one active session of the I/O console.
	 * The access is controlled by the semaphore.
	 */
	Poco::Semaphore m_semaphore;
	/**
	 * Signalize closing of the console.
	 */
	Poco::AtomicCounter m_close;

	std::istream &m_in;
	std::ostream &m_out;
};

}
