#pragma once

#include <iosfwd>

#include <Poco/Mutex.h>
#include <Poco/Semaphore.h>
#include <Poco/SharedPtr.h>
#include <Poco/Timespan.h>

#include <Poco/Net/IPAddress.h>
#include <Poco/Net/Socket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/SocketStream.h>

#include "io/Console.h"
#include "ssl/SSLServer.h"
#include "util/Loggable.h"

namespace Poco {
namespace Net {

class ServerSocket;

}
}

namespace BeeeOn {

class TCPConsoleSessionImpl : public ConsoleSessionImpl, protected Loggable {
public:
	typedef Poco::SharedPtr<TCPConsoleSessionImpl> Ptr;

	TCPConsoleSessionImpl(Poco::Net::Socket &socket);
	~TCPConsoleSessionImpl();

	std::string readUntil(const char c) override;
	std::string readBytes(const unsigned int length) override;
	void print(const std::string &text, bool newline = true) override;
	bool eof() override;

private:
	Poco::Net::Socket m_socket;
	Poco::Net::SocketAddress m_peerAddress;
	Poco::Net::SocketInputStream m_input;
	Poco::Net::SocketOutputStream m_output;
};

/**
 * Console over TCP protocol. It allows to open multiple console
 * connections in parallel if the upper layer can handle them.
 */
class TCPConsole : public Console, protected Loggable {
public:
	TCPConsole();
	~TCPConsole();

	enum {
		DEFAULT_PORT = 6000,
		DEFAULT_BACKLOG = 64,
		DEFAULT_TIMEOUT_MS = 500,
	};

	void setAddress(const std::string &address);
	void setPort(int port);
	void setSendTimeout(const Poco::Timespan &timeout);
	void setRecvTimeout(const Poco::Timespan &timeout);
	void setPollTimeout(const Poco::Timespan &timeout);
	void setBacklog(int backlog);
	void setSSLConfig(SSLServer::Ptr config);

	void startListen();
	void close() override;

protected:
	ConsoleSessionImpl::Ptr openSession() override;
	void startListenUnlocked();

private:
	Poco::Net::IPAddress m_address;
	unsigned int m_port;
	Poco::Timespan m_sendTimeout;
	Poco::Timespan m_recvTimeout;
	Poco::Timespan m_pollTimeout;
	unsigned int m_backlog;
	Poco::SharedPtr<Poco::Net::ServerSocket> m_serverSocket;
	SSLServer::Ptr m_sslConfig;
	Poco::FastMutex m_lock;
	Poco::AtomicCounter m_close;
};

}
