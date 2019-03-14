#include "util/Once.h"

using namespace BeeeOn;
using namespace Poco;
using namespace std;

Once::Once():
	m_triggered(false)
{
}

void Once::execute(const function<void ()> &func)
{
	FastMutex::ScopedLock guard(m_lock);

	if (!m_triggered){
		func();
		m_triggered = true;
	}
}
