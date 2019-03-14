#include <iostream>

#include "io/StreamConsole.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

StreamConsoleSessionImpl::StreamConsoleSessionImpl(
		Semaphore &semaphore,
		istream &in,
		ostream &out):
	m_semaphore(semaphore),
	m_in(in),
	m_out(out)
{
}

StreamConsoleSessionImpl::~StreamConsoleSessionImpl()
{
	m_semaphore.set();
}

string StreamConsoleSessionImpl::readUntil(const char c)
{
	return iosReadUntil(m_in, c);
}

string StreamConsoleSessionImpl::readBytes(const unsigned int length)
{
	return iosReadBytes(m_in, length);
}

void StreamConsoleSessionImpl::print(const string &text, bool newline)
{
	iosPrint(m_out, text, newline);
}

bool StreamConsoleSessionImpl::eof()
{
	return m_in.eof();
}

StreamConsole::StreamConsole(istream &in, ostream &out):
	m_semaphore(1),
	m_close(false),
	m_in(in),
	m_out(out)
{
}

void StreamConsole::close()
{
	m_close = true;
}

ConsoleSessionImpl::Ptr StreamConsole::openSession()
{
	m_semaphore.wait();

	if (m_close)
		return closedSession();

	return new StreamConsoleSessionImpl(m_semaphore, m_in, m_out);
}
