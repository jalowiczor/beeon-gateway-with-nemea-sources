#pragma once

#include <iostream>

#include <Poco/BufferedStreamBuf.h>
#include <Poco/Timespan.h>

struct pollfd;

namespace BeeeOn {

class FdStreamBuf : public Poco::BufferedStreamBuf {
public:
	FdStreamBuf(std::streamsize size, openmode mode);
	FdStreamBuf(int fd, std::streamsize size, openmode mode);
	~FdStreamBuf();

	void setFd(int fd);
	int fd() const;

	int writeToDevice(const char *buffer, std::streamsize size) override;
	int readFromDevice(char *buffer, std::streamsize capacity) override;

	void close();

private:
	int m_fd;
};

class FdStreamIOS : public virtual std::ios {
public:
	enum {
		POLL_READABLE = 0x01,
		POLL_WRITABLE = 0x02,
		POLL_ERROR    = 0x04,
		POLL_HANGUP   = 0x08,
		POLL_INVALID  = 0x10,
		POLL_TIMEOUT  = 0x20,
	};

	FdStreamIOS(std::streamsize size, openmode mode);
	FdStreamIOS(int fd, std::streamsize size, openmode mode);

	int fd() const;
	void setBlocking(bool blocking);
	bool blocking() const;
	void assign(int fd, bool closeOld = true);
	void close();

protected:
	bool poll(struct pollfd &fd, const Poco::Timespan &timeout) const;

protected:
	FdStreamBuf m_buf;
};

class FdInputStream : public virtual FdStreamIOS, public std::istream {
public:
	FdInputStream(std::size_t size = 512);
	FdInputStream(int fd, std::size_t size = 512);

	bool poll(const Poco::Timespan &timeout) const;
};

class FdOutputStream : public virtual FdStreamIOS, public std::ostream {
public:
	FdOutputStream(std::size_t size = 512);
	FdOutputStream(int fd, std::size_t size = 512);

	bool poll(const Poco::Timespan &timeout) const;
};

}
