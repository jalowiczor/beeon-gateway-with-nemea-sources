#pragma once

#include "util/Enum.h"

namespace BeeeOn {

/**
 * @brief OpModeEnum defines a mode of operation in the context of
 * failure. Usually an operation (e.g. with a remote device) can fail.
 * If it fails, we report an error. However, in certain situations, we
 * might want to choose a different strategy of performing that operation
 * (e.g. quickly send multiple request to increase probablity of success).
 * Another possibility is to try again on failure until a timeout occurs.
 */
class OpModeEnum {
public:
	enum Raw {
		TRY_ONCE,       /** Perform operation once.  */
		TRY_HARDER,     /** Choose more reliable strategy. */
		REPEAT_ON_FAIL, /** If the operation fails, try again. */
	};

	static EnumHelper<Raw>::ValueMap &valueMap();
};

typedef Enum<OpModeEnum> OpMode;

}
