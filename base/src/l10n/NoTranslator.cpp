#include "l10n/NoTranslator.h"

using namespace std;
using namespace Poco::Dynamic;
using namespace BeeeOn;

string NoTranslator::formatImpl(
	const string &key,
	const vector<Var> &args)
{
	string str;
	auto it = args.begin();

	for (; it != args.end(); ++it) {
		str += it->toString();
		if ((it + 1) != args.end())
			str += ", ";
	}

	if (str.empty())
		return key;

	return key + " (" + str + ")";
}

Translator *NoTranslatorFactory::create(
		const Locale &,
		const string &)
{
	return new NoTranslator;
}
