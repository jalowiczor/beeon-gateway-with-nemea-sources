#pragma once

#include <string>

#if defined(__GLIBC__) && !defined(__UCLIBC__)
#define BEEEON_BACKTRACE_GLIBC(name, size) \
	void *name[16];                    \
	unsigned int size;
#else
#define BEEEON_BACKTRACE_GLIBC(name, size)
#endif

namespace BeeeOn {

class Backtrace {
public:
	enum {
#if defined(__GLIBC__) && !defined(__UCLIBC__)
	supported = 1
#else
	supported = 0
#endif
	};

	Backtrace(bool empty = false);
	~Backtrace();

	unsigned int size() const;

	std::string operator[] (unsigned int i) const
	{
		return at(i);
	}

	std::string at(unsigned int i) const;
	std::string toString(const std::string &indent = "") const;

	/**
	 * In case of very unusual situations, it is better to
	 * dump the backtrace into an open file instead of standard
	 * logging facilities. This method is intended for such
	 * purposes. It is async-signal-safe.
	 */
	void dump(int fd) const;

protected:
	void fatal() const;

private:
BEEEON_BACKTRACE_GLIBC(m_backtrace, m_backtrace_size)
};

}
