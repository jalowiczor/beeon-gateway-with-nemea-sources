#include <Poco/Clock.h>
#include <Poco/Exception.h>

#include "util/Joiner.h"

using namespace Poco;
using namespace BeeeOn;

Joiner::Joiner(Poco::Thread &thread):
	m_thread(thread),
	m_joined(false)
{
}

static long toMilliseconds(const Timespan &time)
{
	if (time < 0)
		return -1;

	if (time < 1 * Timespan::MILLISECONDS)
		return 1;

	return time.totalMilliseconds();
}

void Joiner::join(const Timespan &timeout)
{
	joinFromMany(toMilliseconds(timeout));
}

bool Joiner::tryJoin(const Timespan &timeout)
{
	try {
		joinFromMany(toMilliseconds(timeout));
		return true;
	}
	catch (const TimeoutException &) {
		return false;
	}
}

static long compensateWaiting(const Clock &started, long ms)
{
	const auto elapsed = started.elapsed();
	const long elapsed_ms = elapsed / 1000;
	const long residual_us = elapsed % 1000;
	const long residualOne = residual_us > 0 ? 1 : 0;

	if (ms - elapsed_ms < 1)
		return residualOne;

	return ms - elapsed_ms + residualOne;
}

void Joiner::joinFromMany(long ms)
{
	if (ms < 0) {
		FastMutex::ScopedLock guard(m_joinLock);
		doJoin(-1);
	}
	else {
		Clock started;
		FastMutex::ScopedLock guard(m_joinLock, ms);

		doJoin(compensateWaiting(started, ms));
	}
}

void Joiner::doJoin(long ms)
{
	if (m_joined)
		return;

	if (ms < 0) {
		m_thread.join();
	}
	else {
		m_thread.join(ms);
	}

	m_joined = true;
}
