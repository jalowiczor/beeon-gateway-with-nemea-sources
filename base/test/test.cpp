#include <iostream>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/RepeatedTest.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextTestProgressListener.h>

#include <Poco/Environment.h>
#include <Poco/Logger.h>
#include <Poco/AutoPtr.h>
#include <Poco/File.h>
#include <Poco/NumberParser.h>
#include <Poco/NullStream.h>
#include <Poco/FileStream.h>
#include <Poco/StringTokenizer.h>
#include <Poco/Util/IniFileConfiguration.h>
#include <Poco/Util/MapConfiguration.h>
#include <Poco/Util/LoggingConfigurator.h>

#include "cppunit/TapOutputter.h"
#include "cppunit/TestTimingListener.h"

#include "util/PosixSignal.h"

using namespace std;
using namespace CppUnit;
using namespace Poco;
using namespace Poco::Util;

void setupLogger(const std::string &path)
{
	File file(path);
	LoggingConfigurator logConfig;

	if (!file.exists()) {
		AutoPtr<MapConfiguration> config(new MapConfiguration);

		logConfig.configure(config);
		cerr << "failed to locate logging configuration file "
			<< file.path() << endl;
		cerr << "try: ./test-suite <path-to>/logging.ini" << endl;
	}
	else {
		AutoPtr<IniFileConfiguration> config(
				new IniFileConfiguration(file.path()));
		logConfig.configure(config);
	}
}

static vector<string> parseTapSkips()
{
	StringTokenizer skips(Environment::get("TEST_TAP_SKIP", ""), ",");
	return {skips.begin(), skips.end()};
}

static int runStandard(Test *suite, const string &format, bool progress, ostream &timing)
{
	TestRunner runner;
	TestResult controller;
	TestResultCollector collector;
	TextTestProgressListener progressListener;
	BeeeOn::TestTimingListener timingListener(timing);

	runner.addTest(suite);
	controller.addListener(&collector);
	controller.addListener(&timingListener);

	if (progress)
		controller.addListener(&progressListener);

	runner.run(controller);

	if (format == "human") {
		CompilerOutputter outputter(&collector, cout);
		outputter.write();
	}
	else if (format == "xml") {
		XmlOutputter outputter(&collector, cout);
		outputter.write();
	}
	else if (format == "tap") {
		BeeeOn::TapOutputter outputter(&collector, cout);

		for (const auto &name : parseTapSkips())
			outputter.skip(name);

		outputter.write();
	}
	else {
		cerr << "unsupported output format: " << format << endl;
		return -1;
	}

	return collector.wasSuccessful()? 0 : 1;
}

int main(int argc, char **argv)
{
	if (Environment::get("TEST_TRAP_FATAL", "yes") != "no")
		BeeeOn::PosixSignal::trapFatal();

	setupLogger(argc > 1? argv[1] : "logging.ini");

	Test *suite = TestFactoryRegistry::getRegistry().makeTest();

	const string &format = Environment::get("TEST_OUTPUT_FORMAT", "human");
	const string &progress = Environment::get("TEST_OUTPUT_PROGRESS", "no");
	const string &timing = Environment::get("TEST_OUTPUT_TIMING", "");

	NullOutputStream noTiming;
	FileOutputStream fileTiming;
	ostream *timingOutput;

	if (!timing.empty()) {
		if (timing == "stdout") {
			timingOutput = &cout;
		}
		else if (timing == "stderr") {
			timingOutput = &cerr;
		}
		else {
			fileTiming.open(timing, ios::trunc);
			timingOutput = &fileTiming;
		}
	}
	else {
		timingOutput = &noTiming;
	}

	const string &repeat = Environment::get("TEST_REPEAT", "1");
	Test *test = new RepeatedTest(suite, NumberParser::parseUnsigned(repeat));

	return runStandard(test, format, NumberParser::parseBool(progress), *timingOutput);
}
