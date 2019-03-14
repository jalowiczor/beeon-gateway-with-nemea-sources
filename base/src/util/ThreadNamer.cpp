#include <Poco/Thread.h>

#include "util/ThreadNamer.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

ThreadNamer::ThreadNamer(const string &name, bool permanent):
	m_permanent(permanent)
{
	Thread *t = Thread::current();

	if (t == nullptr)
		return;

	if (!m_permanent)
		m_originalName = t->getName();

	t->setName(name);
}

ThreadNamer::~ThreadNamer()
{
	Thread *t = Thread::current();

	if (t == nullptr)
		return;

	if (!m_permanent && !m_originalName.empty())
		t->setName(m_originalName);
}
