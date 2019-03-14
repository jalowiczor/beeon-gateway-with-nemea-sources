#include <Poco/Logger.h>

#include "util/CancellableSet.h"
#include "util/ClassInfo.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

CancellableSet::CancellableSet()
{
}

CancellableSet::~CancellableSet()
{
}

void CancellableSet::manage(Cancellable::Ptr c)
{
	FastMutex::ScopedLock guard(m_lock);

	if (logger().debug())
		logger().debug("manage " + ClassInfo::repr(c.get()), __FILE__, __LINE__);

	m_cancellables.emplace(c);
}

bool CancellableSet::unmanage(Cancellable::Ptr c)
{
	FastMutex::ScopedLock guard(m_lock);

	auto it = m_cancellables.find(c);
	if (it == m_cancellables.end())
		return false;

	if (logger().debug())
		logger().debug("unmanage " + ClassInfo::repr(c.get()), __FILE__, __LINE__);

	m_cancellables.erase(it);
	return true;
}

void CancellableSet::cancel()
{
	FastMutex::ScopedLock guard(m_lock);

	for (auto c : m_cancellables) {
		try {
			if (logger().debug())
				logger().debug("cancelling " + ClassInfo::repr(c.get()), __FILE__, __LINE__);

			c->cancel();
		}
		BEEEON_CATCH_CHAIN(logger())
	}

	m_cancellables.clear();
}
