#pragma once

#include <Poco/SharedPtr.h>
#include <Poco/ThreadPool.h>
#include <Poco/Timespan.h>

#include "util/Loggable.h"

namespace BeeeOn {

class HavingThreadPool : protected virtual Loggable {
public:
	HavingThreadPool();
	virtual ~HavingThreadPool();

	void setMinThreads(int min);
	void setMaxThreads(int max);
	void setThreadIdleTime(const Poco::Timespan &time);

	Poco::ThreadPool &pool();

protected:
	void initPool();

private:
	int m_minThreads;
	int m_maxThreads;
	Poco::Timespan m_threadIdleTime;
	Poco::SharedPtr<Poco::ThreadPool> m_pool;
};

}
