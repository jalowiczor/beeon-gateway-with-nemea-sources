#pragma once

#include <functional>

#include <Poco/Thread.h>

#include "util/AbstractAsyncWork.h"

namespace BeeeOn {

/**
 * @brief Adapter of Poco::Thread to AsyncWork interface. The Thread
 * has no general way how to be stopped and thus the cancel() method
 * is implemented as a blocking join().
 */
template <typename Result = Poco::Void>
class ThreadWrapperAsyncWork : public AbstractAsyncWork<Result> {
public:
	typedef Poco::SharedPtr<ThreadWrapperAsyncWork<Result>> Ptr;

	ThreadWrapperAsyncWork(Poco::Thread &thread);

	bool tryJoin(const Poco::Timespan &timeout) override;
	void cancel() override;

	Poco::Thread &thread();

private:
	Poco::Thread &m_thread;
};

template <typename Result>
ThreadWrapperAsyncWork<Result>::ThreadWrapperAsyncWork(
		Poco::Thread &thread):
	m_thread(thread)
{
}

template <typename Result>
bool ThreadWrapperAsyncWork<Result>::tryJoin(const Poco::Timespan &timeout)
{
	return m_thread.tryJoin(timeout.totalMilliseconds());
}

template <typename Result>
void ThreadWrapperAsyncWork<Result>::cancel()
{
	m_thread.join();
}

template <typename Result>
Poco::Thread &ThreadWrapperAsyncWork<Result>::thread()
{
	return m_thread;
}

}
