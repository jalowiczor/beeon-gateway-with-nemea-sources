#pragma once

#include "l10n/LocaleManager.h"

namespace BeeeOn {

class SystemLocaleManager : public LocaleManager {
public:
	Locale parse(const std::string &input) override;
	Locale chooseBest(const std::vector<std::string> &input) override;
};

}
