#include <Poco/Exception.h>
#include <Poco/Logger.h>

#include <Poco/Net/SecureServerSocket.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketAddress.h>

#include "di/Injectable.h"
#include "io/TCPConsole.h"

BEEEON_OBJECT_BEGIN(BeeeOn, TCPConsole)
BEEEON_OBJECT_CASTABLE(Console)
BEEEON_OBJECT_PROPERTY("address", &TCPConsole::setAddress)
BEEEON_OBJECT_PROPERTY("port", &TCPConsole::setPort)
BEEEON_OBJECT_PROPERTY("sendTimeout", &TCPConsole::setSendTimeout)
BEEEON_OBJECT_PROPERTY("recvTimeout", &TCPConsole::setRecvTimeout)
BEEEON_OBJECT_PROPERTY("pollTimeout", &TCPConsole::setPollTimeout)
BEEEON_OBJECT_PROPERTY("backlog", &TCPConsole::setBacklog)
BEEEON_OBJECT_PROPERTY("eol", &TCPConsole::setEol)
BEEEON_OBJECT_PROPERTY("skipEol", &TCPConsole::setSkipEol)
BEEEON_OBJECT_PROPERTY("prompt", &TCPConsole::setPrompt)
BEEEON_OBJECT_PROPERTY("sslConfig", &TCPConsole::setSSLConfig)
BEEEON_OBJECT_HOOK("done", &TCPConsole::startListen)
BEEEON_OBJECT_END(BeeeOn, TCPConsole)

using namespace std;
using namespace Poco;
using namespace Poco::Net;
using namespace BeeeOn;

TCPConsoleSessionImpl::TCPConsoleSessionImpl(Socket &socket):
	m_socket(socket),
	m_peerAddress(socket.peerAddress()),
	m_input(socket),
	m_output(socket)
{
	logger().notice("accepted connection from "
			+ m_peerAddress.toString(),
			__FILE__, __LINE__);

}

TCPConsoleSessionImpl::~TCPConsoleSessionImpl()
{
	logger().notice("closing connection from "
			+ m_peerAddress.toString(),
			__FILE__, __LINE__);

	m_input.close();
	m_output.close();
	m_socket.close();
}

string TCPConsoleSessionImpl::readUntil(const char c)
{
	return iosReadUntil(m_input, c);
}

string TCPConsoleSessionImpl::readBytes(const unsigned int length)
{
	return iosReadBytes(m_input, length);
}

void TCPConsoleSessionImpl::print(const string &text, bool newline)
{
	iosPrint(m_output, text, newline);
}

bool TCPConsoleSessionImpl::eof()
{
	return m_input.eof();
}

TCPConsole::TCPConsole():
	m_address("127.0.0.1"),
	m_port(DEFAULT_PORT),
	m_sendTimeout(DEFAULT_TIMEOUT_MS * Timespan::MILLISECONDS),
	m_recvTimeout(DEFAULT_TIMEOUT_MS * Timespan::MILLISECONDS),
	m_pollTimeout(DEFAULT_TIMEOUT_MS * Timespan::MILLISECONDS),
	m_backlog(DEFAULT_PORT),
	m_close(false)
{
}

TCPConsole::~TCPConsole()
{
}

void TCPConsole::setAddress(const string &address)
{
	m_address = IPAddress(address);
}

void TCPConsole::setPort(int port)
{
	if (port < 0 || port > 0x0ffff)
		throw InvalidArgumentException("port must be in range 0..65535");

	m_port = port;
}

void TCPConsole::setSendTimeout(const Timespan &timeout)
{
	if (timeout < 0)
		throw InvalidArgumentException("sendTimeout must be non-negative");

	m_sendTimeout = timeout;
}

void TCPConsole::setRecvTimeout(const Timespan &timeout)
{
	if (timeout < 0)
		throw InvalidArgumentException("recvTimeout must be non-negative");

	m_recvTimeout = timeout;
}

void TCPConsole::setPollTimeout(const Timespan &timeout)
{
	m_pollTimeout = timeout;
}

void TCPConsole::setBacklog(int backlog)
{
	if (backlog <= 0)
		throw InvalidArgumentException("backlog must be greater then 0");

	m_backlog = backlog;
}

void TCPConsole::setSSLConfig(SSLServer::Ptr config)
{
	m_sslConfig = config;
}

void TCPConsole::startListen()
{
	FastMutex::ScopedLock guard(m_lock);
	startListenUnlocked();
}

void TCPConsole::startListenUnlocked()
{
	if (!m_serverSocket.isNull())
		return;

	if (m_sslConfig.isNull())
		m_serverSocket = new ServerSocket;
	else
		m_serverSocket = new SecureServerSocket(m_sslConfig->context());

	m_close = false;

	SocketAddress address(m_address, m_port);
	m_serverSocket->bind(address, true);
	m_serverSocket->setReuseAddress(true);
	m_serverSocket->setReusePort(true);
	m_serverSocket->listen(m_backlog);

	logger().information("listening at " + address.toString(),
			__FILE__, __LINE__);
}

ConsoleSessionImpl::Ptr TCPConsole::openSession()
{
	FastMutex::ScopedLock guard(m_lock);
	startListenUnlocked();

	logger().information("waiting for connection...",
			__FILE__, __LINE__);

	while (!m_serverSocket->poll(m_pollTimeout, Socket::SELECT_READ)) {
		if (m_close)
			return closedSession();
	}

	Socket socket = m_serverSocket->acceptConnection();
	socket.setSendTimeout(m_sendTimeout);
	socket.setReceiveTimeout(m_recvTimeout);

	return new TCPConsoleSessionImpl(socket);
}

void TCPConsole::close()
{
	m_close = true;
}
