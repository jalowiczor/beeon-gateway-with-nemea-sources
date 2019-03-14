#include <unistd.h>

#include "io/AutoClose.h"

using namespace BeeeOn;

void FdClose::operator() (const int fd) const
{
	if (fd >= 0)
		::close(fd);
}

FdAutoClose::FdAutoClose(const int fd):
	AutoClose<int, FdClose>(m_fd),
	m_fd(fd)
{
}
