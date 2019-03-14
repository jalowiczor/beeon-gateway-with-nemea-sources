#include <cppunit/extensions/HelperMacros.h>
#include <Poco/Exception.h>

#include "cppunit/BetterAssert.h"
#include "model/GlobalID.h"

using namespace std;

namespace BeeeOn {

class GlobalIDTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(GlobalIDTest);
	CPPUNIT_TEST(testToBytes);
	CPPUNIT_TEST(testFromBytes);
	CPPUNIT_TEST(testFromBytesInvalid);
	CPPUNIT_TEST_SUITE_END();
public:
	void testToBytes();
	void testFromBytes();
	void testFromBytesInvalid();
};

CPPUNIT_TEST_SUITE_REGISTRATION(GlobalIDTest);

static string TEST_UUID = "1bffd7fe-fe95-41ba-a572-fb8cc2e4d002";
static const vector<uint8_t> TEST_BYTES = {0x1b, 0xff, 0xd7, 0xfe,
					   0xfe, 0x95, 0x41, 0xba,
					   0xa5, 0x72, 0xfb, 0x8c,
					   0xc2, 0xe4, 0xd0, 0x02};

void GlobalIDTest::testToBytes()
{
	GlobalID id = GlobalID::parse(TEST_UUID);
	const vector<uint8_t> idBytes = id.toBytes();

	CPPUNIT_ASSERT(idBytes == TEST_BYTES);
}

void GlobalIDTest::testFromBytes()
{
	GlobalID id = GlobalID::fromBytes(TEST_BYTES);

	CPPUNIT_ASSERT_EQUAL(TEST_UUID, id.toString());
}

void GlobalIDTest::testFromBytesInvalid()
{
	vector<uint8_t> invalidBytes(8);

	CPPUNIT_ASSERT_THROW(GlobalID::fromBytes(invalidBytes), Poco::InvalidArgumentException);
}

}
