#pragma once

#include <set>

#include <Poco/Mutex.h>

#include "util/Cancellable.h"
#include "util/Loggable.h"

namespace BeeeOn {

class CancellableSet : public Cancellable, Loggable {
public:
	CancellableSet();
	~CancellableSet();

	/**
	 * @brief Manage the given Cancellable instance.
	 * If CancellableSet::cancel() is called, all managed
	 * instances are cancelled.
	 */
	void manage(Cancellable::Ptr cancellable);

	/**
	 * @brief Stop managing the given Cancellable instance.
	 * @returns true if the instance was managed
	 */
	bool unmanage(Cancellable::Ptr cancellable);

	/**
	 * @brief Cancel all managed Cancellable instances.
	 */
	void cancel() override;

private:
	std::set<Cancellable::Ptr> m_cancellables;
	Poco::FastMutex m_lock;
};

}
