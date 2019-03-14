#include <cppunit/extensions/HelperMacros.h>

#include "util/Once.h"

using namespace std;

namespace BeeeOn {

class OnceTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(OnceTest);
	CPPUNIT_TEST(testOnce);
	CPPUNIT_TEST_SUITE_END();
public:
	void testOnce();
};

CPPUNIT_TEST_SUITE_REGISTRATION(OnceTest);

void OnceTest::testOnce()
{
	Once once;

	int i = 0;
	CPPUNIT_ASSERT_EQUAL(0, i);

	once.execute([&]() {
		i += 1;
	});

	CPPUNIT_ASSERT_EQUAL(1, i);

	for (int k = 0; k < 10; ++k) {
		once.execute([&] {
			i += 1;
		});

		CPPUNIT_ASSERT_EQUAL(1, i);
	}
}

}
