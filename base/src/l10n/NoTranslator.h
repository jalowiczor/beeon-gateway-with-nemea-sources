#pragma once

#include "l10n/Translator.h"

namespace BeeeOn {

/**
 * The NoTranslator returns the key without any formatting.
 */
class NoTranslator : public Translator {
public:
	std::string formatImpl(
		const std::string &key,
		const std::vector<Poco::Dynamic::Var> &args) override;
};

class NoTranslatorFactory : public TranslatorFactory {
public:
	Translator *create(
		const Locale &locale,
		const std::string &name = "") override;
};

}
