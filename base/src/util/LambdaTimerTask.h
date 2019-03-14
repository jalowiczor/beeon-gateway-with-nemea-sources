#pragma once

#include <functional>

#include <Poco/AutoPtr.h>
#include <Poco/Util/TimerTask.h>

namespace BeeeOn {

class LambdaTimerTask : public Poco::Util::TimerTask {
public:
	typedef std::function<void(void)> TaskCall;
	typedef Poco::AutoPtr<LambdaTimerTask> Ptr;

	LambdaTimerTask(TaskCall call);

	void run() override;
private:
	TaskCall m_taskCall;
};

}
