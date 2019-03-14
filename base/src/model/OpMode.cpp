#include "model/OpMode.h"

using namespace BeeeOn;

EnumHelper<OpModeEnum::Raw>::ValueMap &OpModeEnum::valueMap()
{
	static EnumHelper<OpModeEnum::Raw>::ValueMap valueMap = {
		{OpModeEnum::TRY_ONCE,       "try_once"},
		{OpModeEnum::TRY_HARDER,     "try_harder"},
		{OpModeEnum::REPEAT_ON_FAIL, "repeat_on_fail"},
	};

	return valueMap;
}
