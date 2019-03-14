#pragma once

#include "util/Enum.h"

namespace BeeeOn {

/**
 * @brief Severity represents a level of importance of a message.
 * Currently, only 3 levels are supported.
 */
class SeverityEnum {
public:
	enum Raw {
		INFO,
		WARNING,
		ERROR,
	};

	static EnumHelper<Raw>::ValueMap &valueMap();
};

typedef Enum<SeverityEnum> Severity;

}
