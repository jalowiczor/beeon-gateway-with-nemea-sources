#pragma once

#include "util/AsyncExecutor.h"
#include "util/Loggable.h"
#include "util/ThreadRecursionProtector.h"

namespace BeeeOn {

/**
 * @brief Implementation of AsyncExecutor that executes the tasks directly
 * in the current thread. Thus, the execution is in fact not asynchronous.
 * The purpose of the NonAsyncExecutor is to provide a way how to deal with
 * delegation of asynchronous processing (that is already in a separate thread)
 * to another AsyncExecutor-based system.
 */
class NonAsyncExecutor : public AsyncExecutor, Loggable {
public:
	typedef Poco::SharedPtr<NonAsyncExecutor> Ptr;

	void invoke(std::function<void()> f) override;

private:
	ThreadRecursionProtector m_protector;
};

}
