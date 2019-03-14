#pragma once

#include <string>

namespace BeeeOn {

/*
 * @brief The class is intended to contain a specific
 * ModuleType Type name.
 */
class CustomTypeID {
public:
	enum {
		MAX_LENGTH = 64
	};

	CustomTypeID();
	CustomTypeID(const std::string &id);

	std::string toString() const;

	static CustomTypeID parse(const std::string &input);

	bool isNull() const;

	bool operator ==(const CustomTypeID &id) const;
	bool operator !=(const CustomTypeID &id) const;
	bool operator <(const CustomTypeID &id) const;
	bool operator >(const CustomTypeID &id) const;
	bool operator <=(const CustomTypeID &id) const;
	bool operator >=(const CustomTypeID &id) const;

private:
	std::string m_value;
};

}
