#include <string>

#include <Poco/Exception.h>
#include <Poco/Thread.h>

#include "util/ThreadRecursionProtector.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

ThreadRecursionProtector::ThreadRecursionProtector()
{
}

void ThreadRecursionProtector::enter(const char *file, int line)
{
	if (m_protect.get()) {
		throw IllegalStateException(
			"thread " + to_string(Thread::currentTid())
			+ " attempts to recurse at "
			+ string(file ? file : "?")
			+ ":" + to_string(line));
	}

	m_protect.get() = true;
}

void ThreadRecursionProtector::leave()
{
	m_protect.get() = false;
}

ThreadRecursionProtector::Guard::Guard(
		ThreadRecursionProtector &protector,
		const char *file,
		int line):
	m_protector(protector)
{
	m_protector.enter(file, line);
}

ThreadRecursionProtector::Guard::~Guard()
{
	m_protector.leave();
}
