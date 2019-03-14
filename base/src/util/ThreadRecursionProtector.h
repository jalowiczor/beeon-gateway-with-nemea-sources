#pragma once

#include <Poco/ThreadLocal.h>

namespace BeeeOn {

/**
 * @brief ThreadRecursionProtector allows to prevent a recursive
 * or repetitive execution of a code. The protection is implemented
 * via methods enter() and leave(). To simplify using of the
 * ThreadRecursionProtector, the ThreadRecursionProtector::Guard
 * is to be used.
 */
class ThreadRecursionProtector {
public:
	/**
	 * @brief Guard calls ThreadRecursionProtector::enter()
	 * from constructor and ThreadRecursionProtector::leave()
	 * from its destructor.
	 */
	class Guard {
	public:
		Guard(
			ThreadRecursionProtector &protector,
			const char *file = 0,
			int line = 0);
		~Guard();

	private:
		ThreadRecursionProtector &m_protector;
	};

	ThreadRecursionProtector();

	/**
	 * @brief Mark location where the recursion is possible.
	 * If the enter is called again, it would fail.
	 */
	void enter(const char *file = 0, int line = 0);

	/**
	 * @brief Leave location where the recursion was possible.
	 * Now, the ThreadRecursionProtector::enter() would not
	 * fail.
	 */
	void leave();

private:
	Poco::ThreadLocal<bool> m_protect;
};

}
