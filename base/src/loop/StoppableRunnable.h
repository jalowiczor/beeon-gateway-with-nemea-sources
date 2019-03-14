#pragma once

#include <Poco/Runnable.h>
#include <Poco/SharedPtr.h>

namespace BeeeOn {

class StoppableRunnable : public Poco::Runnable {
public:
	typedef Poco::SharedPtr<StoppableRunnable> Ptr;

	virtual ~StoppableRunnable();

	/**
	 * Stop the runnable. The call should not block.
	 */
	virtual void stop() = 0;
};

}
