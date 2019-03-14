#include <cerrno>
#include <climits>
#include <cstring>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <string>

#include <Poco/Exception.h>

#include "io/FdStream.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

FdStreamBuf::FdStreamBuf(streamsize size, openmode mode):
	BufferedStreamBuf(size, mode),
	m_fd(-1)
{
}

FdStreamBuf::FdStreamBuf(int fd, streamsize size, openmode mode):
	BufferedStreamBuf(size, mode),
	m_fd(fd)
{
}

FdStreamBuf::~FdStreamBuf()
{
	close();
}

void FdStreamBuf::setFd(int fd)
{
	m_fd = fd;
}

int FdStreamBuf::fd() const
{
	return m_fd;
}

void FdStreamBuf::close()
{
	if (m_fd != 0) {
		::close(m_fd);
		m_fd = -1;
	}
}

int FdStreamBuf::writeToDevice(const char *buffer, streamsize size)
{
	if (m_fd < 0)
		return -1;

	if (size > INT_MAX) {
		throw InvalidArgumentException(
			"size " + to_string(size) + " would overflow int"
		);
	}

	ssize_t wlen = ::write(m_fd, buffer, size);
	if (wlen >= 0)
		return wlen;

	if (errno == EAGAIN)
		return 0;

	if (errno == EINTR)
		throw SignalException(string("write: ") + ::strerror(errno));

	throw IOException(string("write: ") + ::strerror(errno));
}

int FdStreamBuf::readFromDevice(char *buffer, streamsize capacity)
{
	if (m_fd < 0)
		return -1;

	if (capacity > INT_MAX) {
		throw InvalidArgumentException(
			"size " + to_string(capacity) + " would overflow int"
		);
	}

	ssize_t rlen = ::read(m_fd, buffer, capacity);
	if (rlen >= 0)
		return rlen;

	if (errno == EAGAIN)
		return 0;

	if (errno == EINTR)
		throw SignalException(string("write: ") + ::strerror(errno));

	throw IOException(string("read: ") + ::strerror(errno));
}

FdStreamIOS::FdStreamIOS(streamsize size, openmode mode):
	m_buf(size, mode)
{
}

FdStreamIOS::FdStreamIOS(int fd, streamsize size, openmode mode):
	m_buf(fd, size, mode)
{
}

int FdStreamIOS::fd() const
{
	return m_buf.fd();
}

void FdStreamIOS::setBlocking(bool blocking)
{
	const int fd = m_buf.fd();
	int fl = ::fcntl(fd, F_GETFL);
	if (fl < 0)
		throw SystemException(string("fcntl: ") + ::strerror(errno));

	if (blocking) {
		if (!(fl & O_NONBLOCK))
			return;

		fl &= ~O_NONBLOCK;
	}
	else {
		if (fl & O_NONBLOCK)
			return;

		fl |= O_NONBLOCK;
	}

	if (::fcntl(fd, F_SETFL, fl) < 0)
		throw SystemException(string("fcntl: ") + ::strerror(errno));
}

bool FdStreamIOS::blocking() const
{
	const int fd = m_buf.fd();
	int fl = ::fcntl(fd, F_GETFL);
	if (fl < 0)
		throw SystemException(string("fcntl: ") + ::strerror(errno));

	return fd & O_NONBLOCK;
}

void FdStreamIOS::assign(int fd, bool closeOld)
{
	const int oldfd = m_buf.fd();
	m_buf.setFd(fd);

	if (closeOld && oldfd >= 0)
		::close(oldfd);
}

void FdStreamIOS::close()
{
	setstate(ios::eofbit);
	m_buf.close();
}

bool FdStreamIOS::poll(struct pollfd &fd, const Timespan &timeout) const
{
	fd.revents = 0;

	int ret = ::poll(&fd, 1, timeout.totalMilliseconds());
	if (ret < 0) {
		throw SystemException(
			string("poll: ") + strerror(errno));
	}

	if (ret == 0)
		return false;

	if (fd.revents & POLLNVAL)
		throw FileException("invalid file descriptor to poll (POLLNVAL)");
	if (fd.revents & POLLERR)
		throw FileException("special file is broken or closed (POLLERR)");

	// POLLHUP is ignored as it would be detected by reaching EOF

	return true;
}

FdInputStream::FdInputStream(size_t size):
	FdStreamIOS(size, std::ios::in),
	istream(&m_buf)
{
}

FdInputStream::FdInputStream(int fd, size_t size):
	FdStreamIOS(fd, size, std::ios::in),
	istream(&m_buf)
{
}

bool FdInputStream::poll(const Timespan &timeout) const
{
	pollfd pollfd;
	pollfd.fd = fd();
	pollfd.events = POLLIN | POLLRDNORM;

	if (!FdStreamIOS::poll(pollfd, timeout))
		return false;

	return pollfd.revents & (POLLIN | POLLRDNORM | POLLHUP);
}

FdOutputStream::FdOutputStream(size_t size):
	FdStreamIOS(size, std::ios::out),
	ostream(&m_buf)
{
}

FdOutputStream::FdOutputStream(int fd, size_t size):
	FdStreamIOS(fd, size, std::ios::out),
	ostream(&m_buf)
{
}

bool FdOutputStream::poll(const Timespan &timeout) const
{
	pollfd pollfd;
	pollfd.fd = fd();
	pollfd.events = POLLOUT | POLLWRNORM;

	if (!FdStreamIOS::poll(pollfd, timeout))
		return false;

	return pollfd.revents & (POLLOUT | POLLWRNORM | POLLHUP);
}
