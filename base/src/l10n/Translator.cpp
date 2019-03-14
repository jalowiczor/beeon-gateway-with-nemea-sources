#include <typeinfo>

#include <Poco/Any.h>
#include <Poco/Format.h>

#include "l10n/Translator.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

Translator::~Translator()
{
}

string Translator::formatFallback(
	const string &def,
	const vector<Dynamic::Var> &args) const
{
	vector<Any> anyArgs;
	for (const auto &arg : args) {
		if (arg.isEmpty()) {
			anyArgs.emplace_back(string("<null>"));
			continue;
		}

		const auto &type = arg.type();

		if (type == typeid(bool))
			anyArgs.emplace_back(arg.convert<bool>());
		if (type == typeid(char))
			anyArgs.emplace_back(arg.convert<char>());
		else if (type == typeid(unsigned char))
			anyArgs.emplace_back(arg.convert<unsigned char>());
		else if (type == typeid(int))
			anyArgs.emplace_back(arg.convert<int>());
		else if (type == typeid(unsigned int))
			anyArgs.emplace_back(arg.convert<unsigned int>());
		else if (type == typeid(short))
			anyArgs.emplace_back(arg.convert<short>());
		else if (type == typeid(unsigned short))
			anyArgs.emplace_back(arg.convert<unsigned short>());
		else if (type == typeid(long))
			anyArgs.emplace_back(arg.convert<long>());
		else if (type == typeid(unsigned long))
			anyArgs.emplace_back(arg.convert<unsigned long>());
		else if (type == typeid(size_t))
			anyArgs.emplace_back(arg.convert<size_t>());
		else if (type == typeid(float))
			anyArgs.emplace_back(arg.convert<float>());
		else if (type == typeid(double))
			anyArgs.emplace_back(arg.convert<double>());
		else if (type == typeid(string) || arg.isString())
			anyArgs.emplace_back(arg.convert<string>());
		else
			anyArgs.emplace_back(arg.toString());
	}

	string output;
	Poco::format(output, def, anyArgs);
	return output;
}

TranslatorFactory::~TranslatorFactory()
{
}
