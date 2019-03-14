#include "l10n/Locale.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

Locale::Locale():
	m_impl(new SystemLocaleImpl)
{
}

string Locale::language() const
{
	return m_impl->language();
}

string Locale::country() const
{
	return m_impl->country();
}

string Locale::displayName() const
{
	return m_impl->displayName();
}

string Locale::toString() const
{
	return m_impl->toString();
}

bool Locale::lessThan(const Locale &other) const
{
	return toString() < other.toString();
}

SharedPtr<LocaleImpl> Locale::impl() const
{
	return m_impl;
}

Locale Locale::system()
{
	return Locale(new SystemLocaleImpl);
}
