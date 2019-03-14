#pragma once

#include <Poco/SharedPtr.h>

namespace BeeeOn {

/**
 * @brief Any class that can be cancelled should inherit from this
 * interface. This allows to maintain a set of cancellable objects
 * together.
 */
class Cancellable {
public:
	typedef Poco::SharedPtr<Cancellable> Ptr;

	virtual ~Cancellable();

	/**
	 * @brief Cancel the operation this object represents.
	 * It should exit immediatelly or at least as soon as possible.
	 */
	virtual void cancel() = 0;
};

}
