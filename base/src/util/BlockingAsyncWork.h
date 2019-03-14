#pragma once

#include <Poco/SingletonHolder.h>

#include "util/AbstractAsyncWork.h"

namespace BeeeOn {

/**
 * @brief BlockingAsyncWork is an adapter of non-asynchronous
 * operations to the AsyncWork interface. It implements a time
 * barrier that just waits until the underlying operation finishes.
 */
template <typename Result = Poco::Void>
class BlockingAsyncWork : public AbstractAsyncWork<Result> {
public:
	typedef Poco::SharedPtr<BlockingAsyncWork<Result>> Ptr;

	/**
	 * @brief It immediately returns true because there is nothing to
	 * wait for. We are already finished when this method can
	 * be called.
	 */
	bool tryJoin(const Poco::Timespan &) override;

	/**
	 * @brief Do nothing because the underlying operation is already
	 * finished at the time we can call this method.
	 */
	void cancel() override;

	/**
	 * @brief BlockingAsyncWork does not have to be created everytime
	 * it is needed because it does not contain any state.
	 *
	 * @returns global singleton of BlockingAsyncWork
	 */
	static typename BlockingAsyncWork<Result>::Ptr instance();
};

template <typename Result>
inline bool BlockingAsyncWork<Result>::tryJoin(const Poco::Timespan &)
{
	return true;
}

template <typename Result>
inline void BlockingAsyncWork<Result>::cancel()
{
}

template <typename Result>
inline typename BlockingAsyncWork<Result>::Ptr BlockingAsyncWork<Result>::instance()
{
	return new BlockingAsyncWork<Result>;
}

template <>
inline BlockingAsyncWork<Poco::Void>::Ptr BlockingAsyncWork<Poco::Void>::instance()
{
	static Poco::SingletonHolder<BlockingAsyncWork::Ptr> holder;

	auto &work = *holder.get();
	if (work.isNull())
		work = new BlockingAsyncWork<Poco::Void>;

	return work;
}

}
