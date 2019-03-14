#include <Poco/Logger.h>

#include "util/NonAsyncExecutor.h"

using namespace std;
using namespace BeeeOn;

void NonAsyncExecutor::invoke(function<void()> f)
{
	ThreadRecursionProtector::Guard guard(m_protector, __FILE__, __LINE__);

	try {
		f();
	}
	BEEEON_CATCH_CHAIN(logger())
}
