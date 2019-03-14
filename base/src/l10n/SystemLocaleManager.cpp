#include "di/Injectable.h"
#include "l10n/SystemLocaleManager.h"

BEEEON_OBJECT_BEGIN(BeeeOn, SystemLocaleManager)
BEEEON_OBJECT_CASTABLE(LocaleManager)
BEEEON_OBJECT_END(BeeeOn, SystemLocaleManager)

using namespace std;
using namespace BeeeOn;

Locale SystemLocaleManager::parse(const string &)
{
	return Locale();
}

Locale SystemLocaleManager::chooseBest(const vector<string> &)
{
	return Locale();
}
