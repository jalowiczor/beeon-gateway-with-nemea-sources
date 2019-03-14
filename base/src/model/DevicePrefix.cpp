#include "DevicePrefix.h"

using namespace BeeeOn;

DevicePrefixNamesInitializer::DevicePrefixNamesInitializer(
		const ValueMap &map):
	EnumNamesInitializer<DevicePrefix::Raw>(customNamesMap(map))
{
}

DevicePrefixNamesInitializer::NamesMap DevicePrefixNamesInitializer::customNamesMap(
		const ValueMap &map)
{
	NamesMap namesMap = initNamesMap(map);

	namesMap.emplace("Fitprotocol", map.find(DevicePrefix::PREFIX_FITPROTOCOL));
	namesMap.emplace("PressureSensor", map.find(DevicePrefix::PREFIX_PRESSURE_SENSOR));
	namesMap.emplace("pdev", map.find(DevicePrefix::PREFIX_PRESSURE_SENSOR));
	namesMap.emplace("VirtualDevice", map.find(DevicePrefix::PREFIX_VIRTUAL_DEVICE));
	namesMap.emplace("VPT", map.find(DevicePrefix::PREFIX_VPT));
	namesMap.emplace("OpenHAB", map.find(DevicePrefix::PREFIX_OPENHAB));
	namesMap.emplace("Bluetooth", map.find(DevicePrefix::PREFIX_BLUETOOTH));
	namesMap.emplace("BelkinWemo", map.find(DevicePrefix::PREFIX_BELKIN_WEMO));
	namesMap.emplace("Z-Wave", map.find(DevicePrefix::PREFIX_ZWAVE));
	namesMap.emplace("Jablotron", map.find(DevicePrefix::PREFIX_JABLOTRON));
	namesMap.emplace("IQRF", map.find(DevicePrefix::PREFIX_IQRF));
	namesMap.emplace("LEDs", map.find(DevicePrefix::PREFIX_LEDS));
	namesMap.emplace("Philips Hue", map.find(DevicePrefix::PREFIX_PHILIPS_HUE));
	namesMap.emplace("BeeWi", map.find(DevicePrefix::PREFIX_BLE_SMART));

	return namesMap;
}

EnumHelper<DevicePrefixEnum::Raw>::ValueMap
&DevicePrefixEnum::valueMap()
{
	static EnumHelper<DevicePrefixEnum::Raw>::ValueMap valueMap = {
		{DevicePrefixEnum::PREFIX_INVALID, "invalid"},
		{DevicePrefixEnum::PREFIX_FITPROTOCOL, "fitp"},
		{DevicePrefixEnum::PREFIX_PRESSURE_SENSOR, "psdev"},
		{DevicePrefixEnum::PREFIX_VIRTUAL_DEVICE, "vdev"},
		{DevicePrefixEnum::PREFIX_VPT, "vpt"},
		{DevicePrefixEnum::PREFIX_OPENHAB, "open_hab"},
		{DevicePrefixEnum::PREFIX_BLUETOOTH, "bluetooth"},
		{DevicePrefixEnum::PREFIX_BELKIN_WEMO, "belkin_wemo"},
		{DevicePrefixEnum::PREFIX_ZWAVE, "zwave"},
		{DevicePrefixEnum::PREFIX_JABLOTRON, "jablotron"},
		{DevicePrefixEnum::PREFIX_IQRF, "iqrf"},
		{DevicePrefixEnum::PREFIX_LEDS, "leds"},
		{DevicePrefixEnum::PREFIX_PHILIPS_HUE, "philips_hue"},
		{DevicePrefixEnum::PREFIX_BLE_SMART, "ble_smart"},
	};

	return valueMap;
}
