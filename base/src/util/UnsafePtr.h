#pragma once

#include <algorithm>

#include <Poco/Exception.h>

namespace BeeeOn {

/**
 * Wrapper around plain pointers to avoid null-pointer access faults.
 * It does NOT free the owning pointer.
 */
template <typename T>
class UnsafePtr {
public:
	UnsafePtr():
		m_p(nullptr)
	{
	}

	UnsafePtr(T *p):
		m_p(p)
	{
	}

	UnsafePtr(const UnsafePtr &ptr):
		m_p(ptr.m_p)
	{
	}

	template <typename Other>
	UnsafePtr(const UnsafePtr<Other> &ptr):
		m_p(const_cast<Other *>(ptr.get()))
	{
	}

	~UnsafePtr()
	{
		m_p = nullptr;
	}

	void clear()
	{
		m_p = nullptr;
	}

	UnsafePtr &assign(T *p)
	{
		m_p = p;
		return *this;
	}

	UnsafePtr &assign(const UnsafePtr &p)
	{
		m_p = p.m_p;
		return *this;
	}

	template <typename Other>
	UnsafePtr &assign(const UnsafePtr<Other> &p)
	{
		UnsafePtr other(p);
		swap(other);
		return *this;
	}

	UnsafePtr &operator =(T *p)
	{
		return assign(p);
	}

	UnsafePtr &operator =(const UnsafePtr &p)
	{
		return assign(p);
	}

	template <typename Other>
	UnsafePtr &operator =(const UnsafePtr<Other> &p)
	{
		return assign<Other>(p);
	}

	void swap(UnsafePtr &p)
	{
		std::swap(m_p, p.m_p);
	}

	template <typename Other>
	UnsafePtr<Other> cast() const
	{
		Other *other = dynamic_cast<Other *>(m_p);
		if (other)
			return UnsafePtr<Other>(other);

		return UnsafePtr<Other>();
	}

	template <typename Other>
	UnsafePtr<Other> unsafeCast() const
	{
		Other *other = static_cast<Other *>(m_p);
		return UnsafePtr<Other>(other);
	}

	T *operator ->()
	{
		return deref();
	}

	const T *operator ->() const
	{
		return deref();
	}

	T &operator *()
	{
		return *deref();
	}

	const T &operator *() const
	{
		return *deref();
	}

	T *get()
	{
		return m_p;
	}

	const T *get() const
	{
		return m_p;
	}

	operator T* ()
	{
		return m_p;
	}

	operator const T*() const
	{
		return m_p;
	}

	bool isNull() const
	{
		return m_p == nullptr;
	}

	bool operator !() const
	{
		return !isNull();
	}

	bool operator ==(const UnsafePtr &p) const
	{
		return get() == p.get();
	}

	bool operator ==(const T *p) const
	{
		return get() == p;
	}

	bool operator ==(T *p) const
	{
		return get() == p;
	}

	bool operator !=(const UnsafePtr &p) const
	{
		return get() != p.get();
	}

	bool operator !=(const T *p) const
	{
		return get() != p;
	}

	bool operator !=(T *p) const
	{
		return get() != p;
	}

	bool operator <(const UnsafePtr &p) const
	{
		return get() < p.get();
	}

	bool operator <(const T *p) const
	{
		return get() < p;
	}

	bool operator <(T *p) const
	{
		return get() < p;
	}

	bool operator <=(const UnsafePtr &p) const
	{
		return get() <= p.get();
	}

	bool operator <=(const T *p) const
	{
		return get() <= p;
	}

	bool operator <=(T *p) const
	{
		return get() <= p;
	}

	bool operator >(const UnsafePtr &p) const
	{
		return get() > p.get();
	}

	bool operator >(const T *p) const
	{
		return get() > p;
	}

	bool operator >(T *p) const
	{
		return get() > p;
	}

	bool operator >=(const UnsafePtr &p) const
	{
		return get() >= p.get();
	}

	bool operator >=(const T *p) const
	{
		return get() >= p;
	}

	bool operator >=(T *p) const
	{
		return get() >= p;
	}

protected:
	T *deref() const
	{
		if (m_p == nullptr)
			throw Poco::NullPointerException();

		return m_p;
	}

private:
	T *m_p;
};

template <typename T>
inline void swap(UnsafePtr<T> &a, UnsafePtr<T> &b)
{
	a.swap(b);
}

}
