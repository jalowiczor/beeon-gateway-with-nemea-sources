#pragma once

#include <Poco/Exception.h>
#include <Poco/Mutex.h>

#include "util/AsyncWork.h"

namespace BeeeOn {

/**
 * @brief AbstractAsyncWork provides a generic implementation of method
 * result() and a supplementary method setResult(). The specialization
 * for Poco::Void does not allow to set anything and throws exceptions.
 * The result can be set only once.
 */
template <typename Result = Poco::Void>
class AbstractAsyncWork : public AsyncWork<Result> {
public:
	typedef Poco::SharedPtr<AbstractAsyncWork<Result>> Ptr;

	/**
	 * @brief Set result of the operation. The result can
	 * be set only once.
	 */
	void setResult(const Result &result);

	/**
	 * @returns null or the result of the operation
	 */
	Poco::Nullable<Result> result() const override;

private:
	mutable Poco::FastMutex m_lock;
	Poco::Nullable<Result> m_result;
};

template <>
inline void AbstractAsyncWork<Poco::Void>::setResult(const Poco::Void &)
{
	throw Poco::NotImplementedException("cannot set result for Void operation");
}

template <>
inline Poco::Nullable<Poco::Void> AbstractAsyncWork<Poco::Void>::result() const
{
	poco_assert(m_result.isNull());
	return m_result;
}

template <typename Result>
inline void AbstractAsyncWork<Result>::setResult(const Result &result)
{
	Poco::FastMutex::ScopedLock guard(m_lock);

	if (!m_result.isNull())
		throw Poco::IllegalStateException("result is already set");

	m_result = result;
}

template <typename Result>
inline Poco::Nullable<Result> AbstractAsyncWork<Result>::result() const
{
	Poco::FastMutex::ScopedLock guard(m_lock);

	return m_result;
}

}

