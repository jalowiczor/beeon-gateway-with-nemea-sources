#pragma once

#include <functional>
#include <string>

#include <Poco/Event.h>
#include <Poco/Logger.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/Thread.h>

#include "util/AbstractAsyncWork.h"
#include "util/Joiner.h"
#include "util/Loggable.h"
#include "util/ThreadNamer.h"

namespace BeeeOn {

/**
 * @brief Implementation of the AsyncWork interface that executes
 * a given function once after the the given delay unless it is
 * cancelled.
 */
template <typename Result = Poco::Void>
class DelayedAsyncWork : public AbstractAsyncWork<Result>, Loggable {
public:
	typedef Poco::SharedPtr<DelayedAsyncWork<Result>> Ptr;
	typedef std::function<void(DelayedAsyncWork<Result> &)> Call;

	DelayedAsyncWork(
		Call f,
		const Poco::Timespan &delay);
	DelayedAsyncWork(
		Call f,
		Call cancelled,
		const Poco::Timespan &delay);

	bool tryJoin(const Poco::Timespan &timeout) override;
	void cancel() override;

protected:
	void run();

private:
	Call m_f;
	Call m_cancelled;
	Poco::Timespan m_delay;
	Poco::Event m_event;
	Poco::RunnableAdapter<DelayedAsyncWork> m_runnable;
	Poco::Thread m_thread;
	Joiner m_finished;
};

template <typename Result>
DelayedAsyncWork<Result>::DelayedAsyncWork(
		Call f,
		const Poco::Timespan &delay):
	DelayedAsyncWork<Result>(f, [](DelayedAsyncWork<Result> &){}, delay)
{
}

template <typename Result>
DelayedAsyncWork<Result>::DelayedAsyncWork(
		Call f,
		Call cancelled,
		const Poco::Timespan &delay):
	m_f(f),
	m_cancelled(cancelled),
	m_delay(delay),
	m_runnable(*this, &DelayedAsyncWork<Result>::run),
	m_finished(m_thread)
{
	m_thread.start(m_runnable);
}

template <typename Result>
bool DelayedAsyncWork<Result>::tryJoin(const Poco::Timespan &timeout)
{
	return m_finished.tryJoin(timeout);
}

template <typename Result>
void DelayedAsyncWork<Result>::cancel()
{
	m_event.set();
	m_finished.join();
}

template <typename Result>
void DelayedAsyncWork<Result>::run()
{
	ThreadNamer namer("delayed-by-" + std::to_string(m_delay.totalMilliseconds()));

	if (m_event.tryWait(m_delay.totalMilliseconds())) {
		m_cancelled(*this);
		return; // cancelled
	}

	try {
		m_f(*this); // not cancellable, must not sleep
	}
	BEEEON_CATCH_CHAIN(logger())
}

}
