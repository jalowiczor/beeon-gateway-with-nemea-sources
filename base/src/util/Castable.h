#pragma once

#include <typeinfo>

namespace BeeeOn {

class Castable {
public:
	template <typename T>
	bool is()
	{
		return dynamic_cast<T *>(this) != nullptr;
	}

	template <typename T>
	bool is() const
	{
		return dynamic_cast<const T *>(this) != nullptr;
	}

	template<typename C>
	const C &cast() const
	{
		return dynamic_cast<const C &>(*this);
	}

	template<typename C>
	C &cast()
	{
		return dynamic_cast<C &>(*this);
	}

protected:
	virtual ~Castable()
	{
	}
};

}
