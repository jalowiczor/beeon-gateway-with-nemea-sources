#include "util/LambdaTimerTask.h"

using namespace BeeeOn;

void LambdaTimerTask::run()
{
	m_taskCall();
}

LambdaTimerTask::LambdaTimerTask(TaskCall call)
{
	m_taskCall = call;
}
