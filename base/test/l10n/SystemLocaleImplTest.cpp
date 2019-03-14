#include <locale>
#include <cppunit/extensions/HelperMacros.h>

#include "cppunit/BetterAssert.h"
#include "l10n/LocaleImpl.h"

using namespace std;
using namespace Poco;

namespace BeeeOn {

class SystemLocaleImplTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(SystemLocaleImplTest);
	CPPUNIT_TEST(testLanguage);
	CPPUNIT_TEST(testCountry);
	CPPUNIT_TEST(testDisplayName);
	CPPUNIT_TEST(testToString);
	CPPUNIT_TEST_SUITE_END();
public:
	void testLanguage();
	void testCountry();
	void testDisplayName();
	void testToString();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SystemLocaleImplTest);

void SystemLocaleImplTest::testLanguage()
{
	SystemLocaleImpl locale;
	CPPUNIT_ASSERT_EQUAL("C", locale.language());
}

void SystemLocaleImplTest::testCountry()
{
	SystemLocaleImpl locale;
	CPPUNIT_ASSERT_EQUAL("", locale.country());
}

void SystemLocaleImplTest::testDisplayName()
{
	SystemLocaleImpl locale;
	CPPUNIT_ASSERT_EQUAL("C", locale.displayName());
}

void SystemLocaleImplTest::testToString()
{
	SystemLocaleImpl locale;
	CPPUNIT_ASSERT_EQUAL("C", locale.toString());
}

}
