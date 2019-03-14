#include "model/Severity.h"

using namespace BeeeOn;

EnumHelper<SeverityEnum::Raw>::ValueMap
&SeverityEnum::valueMap()
{
	static EnumHelper<SeverityEnum::Raw>::ValueMap valueMap = {
		{SeverityEnum::INFO, "info"},
		{SeverityEnum::WARNING, "warn"},
		{SeverityEnum::ERROR, "error"},
	};

	return valueMap;
}
