#pragma once

#include <string>

namespace BeeeOn {

/**
 * @brief Name the current thread. The name can be assigned permanently
 * or just until the destructor is called.
 */
class ThreadNamer {
public:
	ThreadNamer(const std::string &name, bool permanent = false);
	~ThreadNamer();

private:
	std::string m_originalName;
	bool m_permanent;
};

}
