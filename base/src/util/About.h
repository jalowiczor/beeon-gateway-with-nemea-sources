#pragma once

#include <string>

namespace BeeeOn {

struct About {
	unsigned int requirePocoVersion = 0;
	unsigned int recommendPocoVersion = 0;
	std::string version;
	std::string description;
};

}
