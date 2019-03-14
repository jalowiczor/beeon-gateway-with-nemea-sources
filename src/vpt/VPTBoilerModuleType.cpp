#include "vpt/VPTBoilerModuleType.h"

using namespace BeeeOn;

EnumHelper<VPTBoilerModuleTypeEnum::Raw>::ValueMap
&VPTBoilerModuleTypeEnum::valueMap()
{
	static EnumHelper<VPTBoilerModuleTypeEnum::Raw>::ValueMap valueMap = {
		{MOD_BOILER_STATUS, "MOD_BOILER_STATUS"},
		{MOD_BOILER_MODE, "MOD_BOILER_MODE"},
		{MOD_CURRENT_WATER_TEMPERATURE, "MOD_CURRENT_WATER_TEMPERATURE"},
		{MOD_CURRENT_OUTSIDE_TEMPERATURE, "MOD_CURRENT_OUTSIDE_TEMPERATURE"},
		{MOD_AVERAGE_OUTSIDE_TEMPERATURE, "MOD_AVERAGE_OUTSIDE_TEMPERATURE"},
		{MOD_CURRENT_BOILER_PERFORMANCE, "MOD_CURRENT_BOILER_PERFORMANCE"},
		{MOD_CURRENT_BOILER_PRESSURE, "MOD_CURRENT_BOILER_PRESSURE"},
		{MOD_CURRENT_BOILER_OT_FAULT_FLAGS, "MOD_CURRENT_BOILER_OT_FAULT_FLAGS"},
		{MOD_CURRENT_BOILER_OT_OEM_FAULTS, "MOD_CURRENT_BOILER_OT_OEM_FAULTS"},
	};

	return valueMap;
}
