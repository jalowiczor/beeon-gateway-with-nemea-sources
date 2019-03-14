#pragma once

#include <iosfwd>

#include <cppunit/TestListener.h>

#include <Poco/Timestamp.h>

namespace BeeeOn {

class TestTimingListener : public CppUnit::TestListener {
public:
	TestTimingListener(std::ostream &output);

	void startTest(CppUnit::Test *test) override;
	void endTest(CppUnit::Test *test) override;

private:
	std::ostream &m_output;
	Poco::Timestamp m_start;
};

}
