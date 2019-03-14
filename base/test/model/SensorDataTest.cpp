#include <cppunit/extensions/HelperMacros.h>

#include "cppunit/BetterAssert.h"
#include "model/SensorData.h"

using namespace Poco;
using namespace std;

namespace BeeeOn {

class SensorDataTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(SensorDataTest);
	CPPUNIT_TEST(testCreate);
	CPPUNIT_TEST(testIterator);
	CPPUNIT_TEST(testValueAtIndex);
	CPPUNIT_TEST_SUITE_END();

public:
	void testCreate();
	void testIterator();
	void testValueAtIndex();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SensorDataTest);

void SensorDataTest::testCreate()
{
	const DeviceID id(0x00000000a2010203UL);
	const IncompleteTimestamp timestamp;

	SensorData sensorData;

	sensorData.setDeviceID(id);
	sensorData.setTimestamp(timestamp);

	CPPUNIT_ASSERT_EQUAL(id, sensorData.deviceID());
	CPPUNIT_ASSERT(sensorData.isEmpty());
	CPPUNIT_ASSERT(timestamp == sensorData.timestamp());
	CPPUNIT_ASSERT(sensorData == sensorData);
}

void SensorDataTest::testIterator()
{
	SensorData sensorData;

	sensorData.insertValue({0,1});
	sensorData.insertValue({1,2});
	sensorData.insertValue({2,3});

	CPPUNIT_ASSERT(!sensorData.isEmpty());

	size_t i = 0;
	for (const auto &value : sensorData) {
		CPPUNIT_ASSERT_EQUAL(i, value.moduleID().value());
		CPPUNIT_ASSERT_EQUAL(i+1, value.value());
		i++;
	}
}

void SensorDataTest::testValueAtIndex()
{
	SensorData sensorData;

	sensorData.insertValue({0,1});
	sensorData.insertValue({1,2});

	CPPUNIT_ASSERT(!sensorData.isEmpty());

	CPPUNIT_ASSERT_EQUAL(2, sensorData.size());
	CPPUNIT_ASSERT(SensorValue({0,1}) == sensorData.at(0));
	CPPUNIT_ASSERT(SensorValue({1,2}) == sensorData.at(1));

	CPPUNIT_ASSERT(SensorValue({0,1}) == sensorData[0]);
	CPPUNIT_ASSERT(SensorValue({1,2}) == sensorData[1]);

	sensorData.at(0) = {3,4};
	sensorData[1] = {5,6};

	CPPUNIT_ASSERT(SensorValue({3,4}) == sensorData.at(0));
	CPPUNIT_ASSERT(SensorValue({5,6}) == sensorData[1]);

	CPPUNIT_ASSERT_THROW(sensorData[42], Poco::RangeException);
	CPPUNIT_ASSERT_THROW(sensorData[-1], Poco::RangeException);
}

}
