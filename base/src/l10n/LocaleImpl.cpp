#include <locale>
#include <string>

#include "l10n/LocaleImpl.h"

using namespace std;
using namespace BeeeOn;

LocaleImpl::~LocaleImpl()
{
}

string SystemLocaleImpl::language() const
{
	locale loc;
	const string &name = loc.name();

	size_t pos = name.find("_");
	if (pos == string::npos)
		return name;

	return name.substr(0, pos - 1);
}

string SystemLocaleImpl::country() const
{
	locale loc;
	const string &name = loc.name();

	size_t pos = name.find("_");
	if (pos == string::npos)
		return "";

	size_t dot = name.find(".", pos);
	if (dot == string::npos)
		return name.substr(pos);

	return name.substr(pos, (dot - pos) - 1);
}

string SystemLocaleImpl::displayName() const
{
	locale loc;
	return loc.name();
}

string SystemLocaleImpl::toString() const
{
	locale loc;
	return loc.name();
}
