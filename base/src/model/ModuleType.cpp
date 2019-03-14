#include <cmath>

#include <Poco/NumberFormatter.h>
#include <Poco/RegularExpression.h>
#include <Poco/StringTokenizer.h>

#include "model/ModuleType.h"

using namespace BeeeOn;
using namespace Poco;
using namespace std;

EnumHelper<ModuleType::AttributeEnum::Raw>::ValueMap &ModuleType::AttributeEnum::valueMap()
{
	static EnumHelper<ModuleType::AttributeEnum::Raw>::ValueMap valueMap = {
		{ModuleType::AttributeEnum::ATTR_INNER, "inner"},
		{ModuleType::AttributeEnum::ATTR_MANUAL_ONLY, "manual-only"},
		{ModuleType::AttributeEnum::ATTR_OUTER, "outer"},
		{ModuleType::AttributeEnum::ATTR_CONTROLLABLE, "controllable"},
	};

	return valueMap;
}

EnumHelper<ModuleType::TypeEnum::Raw>::ValueMap &ModuleType::TypeEnum::valueMap()
{
	static EnumHelper<ModuleType::TypeEnum::Raw>::ValueMap valueMap = {
		{ModuleType::TypeEnum::TYPE_UNKNOWN, "unknown"},
		{ModuleType::TypeEnum::TYPE_AVAILABILITY, "availability"},
		{ModuleType::TypeEnum::TYPE_BATTERY, "battery"},
		{ModuleType::TypeEnum::TYPE_BITMAP, "bitmap"},
		{ModuleType::TypeEnum::TYPE_BRIGHTNESS, "brightness"},
		{ModuleType::TypeEnum::TYPE_CO2, "co2"},
		{ModuleType::TypeEnum::TYPE_ENUM, "enum"},
		{ModuleType::TypeEnum::TYPE_FIRE, "fire"},
		{ModuleType::TypeEnum::TYPE_HUMIDITY, "humidity"},
		{ModuleType::TypeEnum::TYPE_LUMINANCE, "luminance"},
		{ModuleType::TypeEnum::TYPE_MOTION, "motion"},
		{ModuleType::TypeEnum::TYPE_NOISE, "noise"},
		{ModuleType::TypeEnum::TYPE_ON_OFF, "on_off"},
		{ModuleType::TypeEnum::TYPE_OPEN_CLOSE, "open_close"},
		{ModuleType::TypeEnum::TYPE_PERFORMANCE, "performance"},
		{ModuleType::TypeEnum::TYPE_PRESSURE, "pressure"},
		{ModuleType::TypeEnum::TYPE_RSSI, "rssi"},
		{ModuleType::TypeEnum::TYPE_SECURITY_ALERT, "security_alert"},
		{ModuleType::TypeEnum::TYPE_SHAKE, "shake"},
		{ModuleType::TypeEnum::TYPE_TEMPERATURE, "temperature"},
		{ModuleType::TypeEnum::TYPE_ULTRAVIOLET, "ultraviolet"},
		{ModuleType::TypeEnum::TYPE_POWER, "power"},
		{ModuleType::TypeEnum::TYPE_VOLTAGE, "voltage"},
		{ModuleType::TypeEnum::TYPE_CURRENT, "current"},
		{ModuleType::TypeEnum::TYPE_FREQUENCY, "frequency"},
		{ModuleType::TypeEnum::TYPE_COLOR_TEMPERATURE, "color_temperature"},
		{ModuleType::TypeEnum::TYPE_COLOR, "color"},
		{ModuleType::TypeEnum::TYPE_SMOKE, "smoke"},
		{ModuleType::TypeEnum::TYPE_HEAT, "heat"},
		{ModuleType::TypeEnum::TYPE_PM25, "pm25"},
	};

	return valueMap;
}

ModuleType::Unit::Unit(const Raw &raw):
	Enum<ModuleType::UnitEnum>(raw)
{
}

EnumHelper<ModuleType::UnitEnum::Raw>::ValueMap &ModuleType::UnitEnum::valueMap()
{
	static EnumHelper<ModuleType::UnitEnum::Raw>::ValueMap valueMap = {
		{NONE,       "none"},
		{BINARY,     "binary"},
		{PERCENT,    "percent"},
		{PPM,        "ppm"},
		{LUX,        "lux"},
		{DECIBEL,     "decibel"},
		{HECTOPASCAL, "hectopascal"},
		{CELSIUS,     "celsius"},
		{UVINDEX,     "uvindex"},
		{WATT,        "watt"},
		{VOLT,        "volt"},
		{AMPERE,      "ampere"},
		{HERTZ,       "hertz"},
		{KELVIN,      "kelvin"},
		{UG_M3,       "ug/m3"},
	};

	return valueMap;
}

bool ModuleType::Unit::isValid(double value) const
{
	switch (raw()) {
	case NONE:
		return !std::isnan(value);
	case BINARY:
		return value == 0 || value == 1;
	case PERCENT:
		return value >= 0 && value <= 100;
	case PPM:
		return value >= 0;
	case LUX:
		return value >= 0 && value <= 100000;
	case DECIBEL:
		return !std::isnan(value);
	case HECTOPASCAL:
		return !std::isnan(value);
	case CELSIUS:
		return value >= -273.15;
	case UVINDEX:
		return value >= 0 && value <= 11;
	case WATT:
		return !std::isnan(value);
	case VOLT:
		return !std::isnan(value);
	case AMPERE:
		return !std::isnan(value);
	case HERTZ:
		return !std::isnan(value);
	case KELVIN:
		return value >= 0;
	case UG_M3:
		return value >= 0;
	}

	throw AssertionViolationException(
		"unexpected unit: " + toString());
}

string ModuleType::Unit::symbol(bool plain) const
{
	switch (raw()) {
	case NONE:
	case BINARY:
		return "";
	case PERCENT:
		return "%";
	case PPM:
		return "ppm";
	case LUX:
		return "lux";
	case DECIBEL:
		return "dB";
	case HECTOPASCAL:
		return "hPa";
	case CELSIUS:
		return (plain? "C" : "\u2103");
	case UVINDEX:
		return "";
	case WATT:
		return "W";
	case VOLT:
		return "V";
	case AMPERE:
		return "A";
	case HERTZ:
		return "Hz";
	case KELVIN:
		return "K";
	case UG_M3:
		return "ug/m3";
	}

	throw AssertionViolationException(
		"unexpected unit: " + toString());
}

string ModuleType::Unit::format(double value, bool plain) const
{
	if (raw() == BINARY)
		return value == 0 ? "false" : "true";

	const string val = NumberFormatter::format(value);
	const string sym = symbol(plain);
	const string space = raw() == CELSIUS && !plain ? "" : " ";

	return val + (sym.empty() ? "" : (space + sym));
}

ModuleType::ModuleType():
	m_type(ModuleType::Type::TYPE_UNKNOWN)
{
}

ModuleType::ModuleType(const ModuleType::Type &type,
		const set<ModuleType::Attribute> &attributes):
	m_type(type),
	m_attributes(attributes)
{
	assureValidAttributes(attributes);
}

ModuleType::ModuleType(const ModuleType::Type &type):
	m_type(type)
{
}

ModuleType::ModuleType(const ModuleType::Type &type, const CustomTypeID &customID):
	m_type(type),
	m_customID(customID)
{
	switch (m_type) {
	case ModuleType::Type::TYPE_ENUM:
		break;
	case ModuleType::Type::TYPE_BITMAP:
		break;
	default:
		throw InvalidArgumentException(
			"module type " + m_type.toString() + " does not support type ID");
	}
}

ModuleType::ModuleType(const ModuleType::Type &type, const CustomTypeID &customID,
		const std::set<Attribute> &attributes):
	m_type(type),
	m_attributes(attributes),
	m_customID(customID)
{
	assureValidAttributes(attributes);

	switch (m_type) {
	case ModuleType::Type::TYPE_ENUM:
		break;
	case ModuleType::Type::TYPE_BITMAP:
		break;
	default:
		throw InvalidArgumentException(
			"module type " + m_type.toString() + " does not support type ID");
	}
}

ModuleType::Unit ModuleType::baseUnit() const
{
	switch (m_type.raw()) {
	case Type::TYPE_UNKNOWN:
	case Type::TYPE_BITMAP:
	case Type::TYPE_COLOR:
	case Type::TYPE_ENUM:
		return Unit::NONE;

	case Type::TYPE_AVAILABILITY:
	case Type::TYPE_FIRE:
	case Type::TYPE_MOTION:
	case Type::TYPE_OPEN_CLOSE:
	case Type::TYPE_ON_OFF:
	case Type::TYPE_SECURITY_ALERT:
	case Type::TYPE_SHAKE:
	case Type::TYPE_SMOKE:
	case Type::TYPE_HEAT:
		return Unit::BINARY;

	case Type::TYPE_BATTERY:
	case Type::TYPE_BRIGHTNESS:
	case Type::TYPE_HUMIDITY:
	case Type::TYPE_PERFORMANCE:
	case Type::TYPE_RSSI:
		return Unit::PERCENT;

	case Type::TYPE_CO2:
		return Unit::PPM;

	case Type::TYPE_LUMINANCE:
		return Unit::LUX;

	case Type::TYPE_NOISE:
		return Unit::DECIBEL;

	case Type::TYPE_PRESSURE:
		return Unit::HECTOPASCAL;

	case Type::TYPE_TEMPERATURE:
		return Unit::CELSIUS;

	case Type::TYPE_ULTRAVIOLET:
		return Unit::UVINDEX;

	case Type::TYPE_POWER:
		return Unit::WATT;

	case Type::TYPE_VOLTAGE:
		return Unit::VOLT;

	case Type::TYPE_CURRENT:
		return Unit::AMPERE;

	case Type::TYPE_FREQUENCY:
		return Unit::HERTZ;

	case Type::TYPE_COLOR_TEMPERATURE:
		return Unit::KELVIN;

	case Type::TYPE_PM25:
		return Unit::UG_M3;

	// no default to let compiler catch a missing one
	}

	throw AssertionViolationException(
		"no unit for type " + type().toString());
}

void ModuleType::setType(const ModuleType::Type &type)
{
	m_type = type;
}

ModuleType::Type ModuleType::type() const
{
	return m_type;
}

void ModuleType::setAttributes(const set<ModuleType::Attribute> &attributes)
{
	assureValidAttributes(attributes);
	m_attributes = attributes;
}

set<ModuleType::Attribute> ModuleType::attributes() const
{
	return m_attributes;
}

bool ModuleType::hasCombination(
	const set<Attribute> &attributes,
	const set<Attribute> &check)
{
	for (const auto &one : check) {
		if (attributes.find(one) == attributes.end())
			return false;
	}

	return true;
}

void ModuleType::assureValidAttributes(const set<Attribute> &attributes)
{
	if (hasCombination(attributes,
			{Attribute::ATTR_INNER, Attribute::ATTR_OUTER})) {
		throw InvalidArgumentException(
			"inner and outer cannot be set at once");
	}

	if (attributes.find(Attribute::ATTR_MANUAL_ONLY) != attributes.end()) {
		if (attributes.find(Attribute::ATTR_CONTROLLABLE) == attributes.end()) {
			throw InvalidArgumentException(
				"manual-only attribute requires attribute controllable");
		}
	}
}

bool ModuleType::isControllable() const
{
	return m_attributes.find(Attribute::ATTR_CONTROLLABLE) != m_attributes.end();
}

void ModuleType::setCustomTypeID(CustomTypeID id)
{
	switch (m_type) {
	case ModuleType::Type::TYPE_ENUM:
		break;
	case ModuleType::Type::TYPE_BITMAP:
		break;
	default:
		throw IllegalStateException(
			"module type " + m_type.toString() + " does not support type ID");
	}

	m_customID = id;
}

CustomTypeID ModuleType::customTypeID() const
{
	return m_customID;
}

ModuleType ModuleType::parse(string input)
{
	set<ModuleType::Attribute> attributes;

	const StringTokenizer tokens(input, ",",
		StringTokenizer::TOK_IGNORE_EMPTY | StringTokenizer::TOK_TRIM);

	for (unsigned int k = 1; k < tokens.count(); k++) {
		ModuleType::Attribute current =
			ModuleType::Attribute::parse(tokens[k]);

		auto it = attributes.find(current);
		if (it != attributes.end()) {
			throw InvalidArgumentException(
				"duplicate attribute '" + input + "'");
		}

		attributes.insert(current);
	}

	assureValidAttributes(attributes);

	RegularExpression::MatchVec matches;
	RegularExpression re("(enum|bitmap):(.+)");

	string typeName;
	string idValue;

	if (re.match(tokens[0], 0, matches) == 3) {
		typeName = tokens[0].substr(matches[1].offset, matches[1].length);
		idValue = tokens[0].substr(matches[2].offset, matches[2].length);
	}
	else {
		typeName = tokens[0];
	}

	const ModuleType::Type type = ModuleType::Type::parse(typeName);

	switch (type) {
	case ModuleType::Type::TYPE_ENUM:
	case ModuleType::Type::TYPE_BITMAP:
		if (idValue.empty())
			throw InvalidArgumentException("missing ID for type " + type.toString());
		break;
	default:
		return ModuleType(type, attributes);
	}

	const auto id = CustomTypeID::parse(idValue);
	return ModuleType(type, id, attributes);
}
