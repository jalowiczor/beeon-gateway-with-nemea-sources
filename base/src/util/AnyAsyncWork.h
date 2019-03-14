#pragma once

#include <Poco/SharedPtr.h>
#include <Poco/Timespan.h>

#include "util/Cancellable.h"

namespace BeeeOn {

/**
 * @brief AnyAsyncWork represents an asynchronous work to be processed.
 * Such work is defined only in terms of execution. It is started somehow
 * (implementation specific) and can be joined or cancelled.
 */
class AnyAsyncWork : public Cancellable {
public:
	typedef Poco::SharedPtr<AnyAsyncWork> Ptr;

	virtual ~AnyAsyncWork();

	/**
	 * @returns true when the time discovery process finishes,
	 * false when the timeout has exceeded
	 */
	virtual bool tryJoin(const Poco::Timespan &timeout) = 0;
};

}
