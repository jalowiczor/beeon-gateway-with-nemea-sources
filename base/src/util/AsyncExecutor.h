#pragma once

#include <functional>

#include <Poco/SharedPtr.h>

namespace BeeeOn {

/**
 * Class for asynchronous executing tasks.
 */
class AsyncExecutor {
public:
	typedef Poco::SharedPtr<AsyncExecutor> Ptr;

	AsyncExecutor();
	virtual ~AsyncExecutor();

	/**
	 * Add task to queue for executing
	 */
	virtual void invoke(std::function<void()> f) = 0;
};

}
