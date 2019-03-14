#pragma once

#include <string>
#include <vector>

#include "l10n/Locale.h"

namespace BeeeOn {

class LocaleManager {
public:
	typedef Poco::SharedPtr<LocaleManager> Ptr;

	virtual ~LocaleManager();

	virtual Locale parse(const std::string &input) = 0;
	virtual Locale chooseBest(const std::vector<std::string> &input) = 0;
};

}
