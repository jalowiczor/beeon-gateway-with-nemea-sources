#pragma once

#include <string>

#include <Poco/Exception.h>
#include <Poco/SharedPtr.h>

#include "l10n/LocaleImpl.h"

namespace BeeeOn {

class Locale {
public:
	/**
	 * Creates system locale.
	 */
	Locale();

	template <typename T>
	Locale(Poco::SharedPtr<T> impl):
		m_impl(impl)
	{
		if (m_impl.isNull())
			throw Poco::BadCastException("failed to cast to LocaleImpl");
	}

	template <typename T>
	Locale(T *impl):
		m_impl(impl)
	{
	}

	std::string language() const;
	std::string country() const;
	std::string displayName() const;
	std::string toString() const;

	bool operator <(const Locale &other) const
	{
		return lessThan(other);
	}

	bool lessThan(const Locale &other) const;

	Poco::SharedPtr<LocaleImpl> impl() const;

	static Locale system();

private:
	Poco::SharedPtr<LocaleImpl> m_impl;
};

}
