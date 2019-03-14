#pragma once

#include <string>
#include <vector>

#include <Poco/ErrorHandler.h>
#include <Poco/Exception.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>

#include "util/About.h"
#include "util/Loggable.h"

namespace Poco {
namespace Util {

class OptionSet;

}
}

namespace BeeeOn {

class DIDaemon : public Poco::Util::ServerApplication {
public:
	DIDaemon(const About &about);
	virtual ~DIDaemon();

	static int up(int argc, char **argv,
		const About &about = About());

protected:
	class UnhandledErrorHandler : public Poco::ErrorHandler, Loggable {
	public:
		virtual ~UnhandledErrorHandler();

		void exception(const Poco::Exception &e) override;
		void exception(const std::exception &e) override;
		void exception() override;
	};

	void initialize(Poco::Util::Application &self) override;
	int main(const std::vector<std::string> &args) override;
	void defineOptions(Poco::Util::OptionSet &options) override;
	void handleHelp(const std::string &name, const std::string &value);
	void handleVersion(const std::string &name, const std::string &value);
	void handleDebugStartup(const std::string &name, const std::string &value);
	void handleDefine(const std::string &name, const std::string &value);
	void handleConfig(const std::string &name, const std::string &value);
	void handleNoEarly(const std::string &name, const std::string &value);
	void startRunner(const std::string &name);
	void printHelp() const;
	void printVersion() const;
	void notifyStarted() const;
	void testPocoCompatibility() const;
	void logStartup() const;
	bool isUnix() const;
	bool helpRequested() const;
	bool versionRequested() const;
	bool noEarlyRequested() const;
	std::vector<std::string> libraryPaths();
	std::string runnerName();
	std::string version() const;

private:
	bool m_helpRequested = false;
	bool m_versionRequested = false;
	bool m_noEarlyRequested = false;
	About m_about;
	UnhandledErrorHandler m_errorHandler;
	Poco::Util::Option m_helpOption;
	Poco::Util::Option m_versionOption;
	Poco::Util::Option m_debugStartupOption;
	Poco::Util::Option m_defineOption;
	Poco::Util::Option m_configOption;
	Poco::Util::Option m_notifyStartedOption;
	Poco::Util::Option m_noEarlyOption;
};

}
