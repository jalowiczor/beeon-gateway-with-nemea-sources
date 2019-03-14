#pragma once

#include <string>
#include <typeinfo>

#include <Poco/SharedPtr.h>

namespace BeeeOn {

class DIWrapper;
class DIWCast;

/**
 * @brief Implementation of DIFactory can manage and create instances
 * by name and automatically (usually based on some configuration)
 * inject dependencies into each instance.
 */
class DIFactory {
public:
	typedef Poco::SharedPtr<DIFactory> Ptr;

	virtual ~DIFactory();

	/**
	 * @brief Create a new instance by name. If detached is true,
	 * the memory of the returned DIWrapper pointer MUST be explicitly
	 * deleted. Otherwise, each created instance is managed by the
	 * DIFactory.
	 */
	DIWrapper *create(
		const std::string &name,
		bool detach = false)
	{
		return createImpl(name, detach);
	}

	/**
	 * @brief Create a new instance by name and perform runtime cast
	 * to the target type if possible.
	 * @see DIFactory::create()
	 */
	template <typename T>
	Poco::SharedPtr<T> create(
		const std::string &name,
		bool detach = false)
	{
		auto wrapper = create(name, detach);
		if (wrapper == nullptr)
			return {};

		return cast<T>(*wrapper);
	}

	/**
	 * @brief Find an existing instance and return it.
	 * @returns found instance or nullptr
	 */
	DIWrapper *find(const std::string &name)
	{
		return findImpl(name);
	}

	/**
	 * @brief Find an existing instance and cast it to the
	 * target type if possible.
	 * @returns found instance or nullptr
	 */
	template <typename T>
	Poco::SharedPtr<T> find(const std::string &name)
	{
		auto wrapper = findImpl(name);
		if (wrapper == nullptr)
			return {};

		return cast<T>(*wrapper);
	}

protected:
	/**
	 * @brief Cast the wrapped instance to the target type.
	 * If the cast is not possible, throw an exception.
	 */
	template <typename T>
	Poco::SharedPtr<T> cast(DIWrapper &wrapper)
	{
		Poco::SharedPtr<T> instance;
		castImpl(typeid(T), wrapper, reinterpret_cast<void *>(&instance));

		return instance;
	}

	/**
	 * @returns DIWrapper that represents the instance
	 * of the given name or nullptr.
	 */
	virtual DIWrapper *findImpl(const std::string &name) = 0;

	/**
	 * @returns DIWrapper that represents the instance
	 * of the given name. If it already exists, return
	 * the existing one.
	 */
	virtual DIWrapper *createImpl(
		const std::string &name,
		bool detach) = 0;

	/**
	 * @brief Perform cast of the given wrapper to the
	 * target of the given type. The target points to
	 * the Poco::SharedPtr<T> where typeid(T) == type.
	 */
	virtual void castImpl(
		const std::type_info &type,
		const DIWrapper &wrapper,
		void *target) = 0;

};

}
