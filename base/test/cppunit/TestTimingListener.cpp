#include <iostream>

#include <cppunit/Test.h>

#include <Poco/Timespan.h>

#include "cppunit/TestTimingListener.h"

using namespace CppUnit;
using namespace Poco;
using namespace BeeeOn;

TestTimingListener::TestTimingListener(std::ostream &output):
	m_output(output)
{
}

void TestTimingListener::startTest(Test *)
{
	m_start.update();
}

void TestTimingListener::endTest(Test *test)
{
	const Timespan elapsed = m_start.elapsed();
	double us = elapsed.totalMicroseconds();

	m_output
		<< test->getName()
		<< "\t"
		<< std::fixed
		<< (us / 1000000.0)
		<< " s"
		<< std::endl;
}
