#pragma once

#include <string>

#include <Poco/NamedMutex.h>
#include <Poco/SharedPtr.h>

#include "util/Loggable.h"

namespace BeeeOn {

/**
 * @brief SingleInstanceChecker can detect another equivalent running
 * application instance. This might prevent running a same kind of
 * application multiple-times.
 *
 * SingleInstanceChecker uses Poco::NamedMutex internally to detect
 * another running application instance having the same name. It tries
 * to lock when the SingleInstanceChecker::check() is called and unlocks
 * upon destruction.
 *
 * There are 3 modes of operation with respect to the situation when
 * another application instance is detected:
 *
 * - fail - fail when another application instance is detected
 * - ignore - ignore when another application instance is detected
 *            and do not touch it in anyway
 * - recover - when another application instance is detected (this might
 *             be a result of unclean application exit), take over the used
 *             lock and do not fail
 */
class SingleInstanceChecker : protected Loggable {
public:
	enum Mode {
		MODE_FAIL,   /**< fail if another instance is detected */
		MODE_IGNORE, /**< ignore when another instance is detected */
		MODE_RECOVER,/**< recover from a false positive failure */
	};

	SingleInstanceChecker();
	~SingleInstanceChecker();

	void setName(const std::string &name);
	void setMode(const std::string &mode);

	/**
	 * @brief Try lock the global mutex. If it successfully locks, jsut
	 * continue. Otherwise, based on the configured mode, react on the
	 * situation by failing, ignoring or recovering.
	 */
	void check();

private:
	std::string m_name;
	Mode m_mode;
	Poco::SharedPtr<Poco::NamedMutex> m_lock;
};

}
