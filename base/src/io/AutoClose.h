#pragma once

#include <Poco/Logger.h>

#include "util/Loggable.h"

namespace BeeeOn {

template <typename Closable>
struct DefaultClose {
	void operator() (Closable &c) const
	{
		c.close();
	}
};

template <typename Closable, typename Close = DefaultClose<Closable>>
class AutoClose : protected Loggable {
public:
	AutoClose(Closable &c):
		m_closable(c)
	{
	}

	/**
	 * Cannot copy because we would otherwise make possible
	 * to cause a double-close failure.
	 */
	AutoClose(const AutoClose &c) = delete;

	~AutoClose()
	{
		try {
			const Close close;
			close(m_closable);

			if (logger().debug()) {
				logger().debug(
					"auto-close successful",
					__FILE__, __LINE__
				);
			}
		}
		catch (...) {
			logger().fatal(
				"auto-close has failed",
				__FILE__, __LINE__
			);
		}
	}

	Closable *operator ->()
	{
		return &m_closable;
	}

	const Closable *operator ->() const
	{
		return &m_closable;
	}

	Closable &operator *()
	{
		return m_closable;
	}

	const Closable &operator *() const
	{
		return m_closable;
	}

private:
	Closable &m_closable;
};

struct FdClose {
	void operator() (const int fd) const;
};

class FdAutoClose : public AutoClose<int, FdClose> {
public:
	FdAutoClose(const int fd);

private:
	int m_fd;
};

}
