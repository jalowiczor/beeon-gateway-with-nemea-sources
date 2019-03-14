#pragma once

#include <functional>

#include <Poco/Mutex.h>

namespace BeeeOn {

/**
 * Helper class providing single execution of some function
 * for a lifetime of this object.
 * E.g. we want to log some event only the first time it occures
 * and not everytime.
 */
class Once {
public:
	Once();

	void execute(const std::function<void ()> &func);

private:
	Poco::FastMutex m_lock;
	bool m_triggered;
};

}
