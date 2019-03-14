#include <Poco/Exception.h>
#include <Poco/File.h>
#include <Poco/Logger.h>
#include <Poco/Path.h>
#include <Poco/String.h>

#include "di/Injectable.h"
#include "util/SingleInstanceChecker.h"

BEEEON_OBJECT_BEGIN(BeeeOn, SingleInstanceChecker)
BEEEON_OBJECT_PROPERTY("name", &SingleInstanceChecker::setName)
BEEEON_OBJECT_PROPERTY("mode", &SingleInstanceChecker::setMode)
BEEEON_OBJECT_HOOK("done", &SingleInstanceChecker::check)
BEEEON_OBJECT_END(BeeeOn, SingleInstanceChecker)

using namespace std;
using namespace Poco;
using namespace BeeeOn;

SingleInstanceChecker::SingleInstanceChecker():
	m_name("application"),
	m_mode(MODE_FAIL)
{
}

SingleInstanceChecker::~SingleInstanceChecker()
{
	if (!m_lock.isNull()) {
		m_lock->unlock();

		try {
			// clean up the lock file if exists
			File tmpFile(Path(Path::temp(), m_name + ".mutex"));
			tmpFile.remove();
		}
		catch (...) {}
	}
}

void SingleInstanceChecker::setName(const string &name)
{
	m_name = name;
}

void SingleInstanceChecker::setMode(const string &mode)
{
	if (!icompare(mode, "fail"))
		m_mode = MODE_FAIL;
	else if (!icompare(mode, "ignore"))
		m_mode = MODE_IGNORE;
	else if (!icompare(mode, "recover"))
		m_mode = MODE_RECOVER;
	else
		throw InvalidArgumentException("unrecognized mode: " + mode);
}

void SingleInstanceChecker::check()
{
	m_lock = new NamedMutex(m_name);

	if (m_lock->tryLock()) {
		logger().information(
			"application instance named '"
			+ m_name + "' would start");
		return;
	}

	logger().warning(
		"it seems that another application instance named '"
		+ m_name + "' is already running");

	switch (m_mode) {
	case MODE_FAIL:
		m_lock = nullptr; // do not unlock
		throw IllegalStateException(
			"another application instanced named '"
			+ m_name + "' is already running");

	case MODE_IGNORE:
		m_lock = nullptr; // do not unlock
		break;

	case MODE_RECOVER:
		while (!m_lock->tryLock())
			m_lock->unlock();

		logger().notice(
			"recovering, application instanced named '"
			+ m_name + "' would start");
		break;
	}
}
