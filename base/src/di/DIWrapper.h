#pragma once

#include <climits>
#include <cmath>
#include <list>
#include <string>
#include <typeinfo>
#include <list>
#include <map>
#include <type_traits>

#include <Poco/SharedPtr.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/Timespan.h>

namespace BeeeOn {

class DependencyInjector;
class DIWrapper;

/**
 * Duplicate injection helper has been detected.
 * This is a user bug, probably a duplicate setter entry.
 */
class DIWDuplicateException final : public std::exception {
public:
	DIWDuplicateException(const std::string &key,
			const std::type_info &method);

	const char *what() const noexcept override;

private:
	std::string m_text;
};

/**
 * Cannot cast in a setter. Probably, it can be solved
 * by a manual specification by BEEEON_OBJECT_CASTABLE.
 */
class DIWCastException final : public std::bad_cast {
public:
	DIWCastException(const std::type_info &from,
			const std::type_info &to);

	const char *what() const noexcept override;

private:
	std::string m_text;
};

class DIWWrongInputException final : public std::exception {
public:
	DIWWrongInputException(const std::string &text);

	const char *what() const noexcept override;

private:
	std::string m_text;
};

/**
 * Common class to be inherited by all setters helpers.
 * The purpose is to be able to have all setters in a single map.
 */
struct DIWMethodHelper {
	virtual ~DIWMethodHelper();

	virtual std::string id() const = 0;

	/**
	 * Get instance of the given wrapper.
	 */
	template <typename T, typename B>
	static B &extractInstance(DIWrapper &w);
};

/**
 * Interface to implementations of text setters.
 */
struct DIWTextSetter : public DIWMethodHelper {
	virtual ~DIWTextSetter();

	std::string id() const override
	{
		return "text";
	}

	virtual void call(DIWrapper &b, const std::string &text) = 0;
};

template <typename T, typename B>
class DIWStringSetter final : public DIWTextSetter {
public:
	typedef void (B::*Setter)(const std::string &);

	DIWStringSetter(Setter setter):
		m_setter(setter)
	{
	}

	void call(DIWrapper &b, const std::string &text) override;
private:
	Setter m_setter;
};

template <typename T, typename B>
class DIWCharSetter final : public DIWTextSetter {
public:
	typedef void (B::*Setter)(const char);

	DIWCharSetter(Setter setter):
		m_setter(setter)
	{
	}

	void call(DIWrapper &b, const std::string &text) override;
private:
	Setter m_setter;
};

/**
 * Interface to implementations of time setters.
 */
struct DIWTimeSetter : public DIWMethodHelper {
	virtual ~DIWTimeSetter();

	std::string id() const override
	{
		return "time";
	}

	virtual void call(DIWrapper &b, const Poco::Timespan &time) = 0;
};

template <typename T, typename B>
class DIWTimespanSetter final : public DIWTimeSetter {
public:
	typedef void (B::*Setter)(const Poco::Timespan &);

	DIWTimespanSetter(Setter setter):
		m_setter(setter)
	{
	}

	void call(DIWrapper &b, const Poco::Timespan &time) override;

private:
	Setter m_setter;
};

/**
 * Interface to implementations of number setters.
 */
struct DIWNumberSetter : public DIWMethodHelper {
	virtual ~DIWNumberSetter();

	std::string id() const override
	{
		return "number";
	}

	virtual void call(DIWrapper &b, double value) = 0;
};

template <typename T, typename B>
class DIWIntSetter final : public DIWNumberSetter {
public:
	typedef void (B::*Setter)(int value);

	DIWIntSetter(Setter setter):
		m_setter(setter)
	{
	}

	void call(DIWrapper &b, double value) override;
private:
	Setter m_setter;
};

template <typename T, typename B>
class DIWDoubleSetter final : public DIWNumberSetter {
public:
	typedef void (B::*Setter)(double value);

	DIWDoubleSetter(Setter setter):
		m_setter(setter)
	{
	}

	void call(DIWrapper &b, double value) override;
private:
	Setter m_setter;
};

template <typename T, typename B>
class DIWBoolSetter final : public DIWNumberSetter {
public:
	typedef void (B::*Setter)(bool value);

	DIWBoolSetter(Setter setter):
		m_setter(setter)
	{
	}

	void call(DIWrapper &b, double value) override;
private:
	Setter m_setter;
};

/**
 * Interface to implementations of object reference setter.
 */
struct DIWRefSetter : public DIWMethodHelper {
	virtual ~DIWRefSetter();

	std::string id() const override
	{
		return "ref";
	}

	virtual void call(DIWrapper &b, DIWrapper &i) = 0;

protected:
	template <typename I>
	Poco::SharedPtr<I> extractTarget(DIWrapper &i) const;
};

/**
 * Implement set via a raw pointer I*.
 */
template <typename T, typename B, typename I>
class DIWRawPtrSetter final : public DIWRefSetter {
public:
	typedef void (B::*Setter)(I *);

	DIWRawPtrSetter(Setter setter):
		m_setter(setter)
	{
	}

	void call(DIWrapper &b, DIWrapper &i) override;
private:
	Setter m_setter;
};

/**
 * Implement set via Poco::SharedPtr<I>.
 */
template <typename T, typename B, typename I>
class DIWSharedPtrSetter final : public DIWRefSetter {
public:
	typedef void (B::*Setter)(Poco::SharedPtr<I>);

	DIWSharedPtrSetter(Setter setter):
		m_setter(setter)
	{
	}

	void call(DIWrapper &b, DIWrapper &i) override;
private:
	Setter m_setter;
};

struct DIWListSetter : public DIWMethodHelper {
	virtual ~DIWListSetter();

	std::string id() const override
	{
		return "list";
	}

	virtual void call(DIWrapper &b, const std::list<Poco::Dynamic::Var> &l) = 0;
};

template <typename T, typename B>
class DIWStringListSetter final : public DIWListSetter {
public:
	typedef void (B::*Setter)(const std::list<std::string> &);

	DIWStringListSetter(Setter setter):
		m_setter(setter)
	{
	}

	void call(DIWrapper &b, const std::list<Poco::Dynamic::Var> &l) override;
private:
	Setter m_setter;
};

struct DIWMapSetter : public DIWMethodHelper {
	virtual ~DIWMapSetter();

	std::string id() const override
	{
		return "map";
	}

	virtual void call(DIWrapper &b,
			  const std::map<Poco::Dynamic::Var, Poco::Dynamic::Var> &m) = 0;
};

template <typename T, typename B>
class DIWStringStringMapSetter final : public DIWMapSetter {
public:
	typedef void (B::*Setter)(const std::map<std::string, std::string> &);

	DIWStringStringMapSetter(Setter setter):
		m_setter(setter)
	{
	}

	void call(DIWrapper &b,
		  const std::map<Poco::Dynamic::Var, Poco::Dynamic::Var> &m) override;
private:
	Setter m_setter;
};

/**
 * Interface to a casting implementation. There is also a static
 * registry of possible casts so anybody can access it.
 * All casts register by DIWCast::add() are automatically deleted
 * on exit.
 */
struct DIWCast {
	virtual ~DIWCast();
	virtual bool wouldCast(const std::type_info &info) = 0;
	virtual bool isSame(const DIWrapper &wrapper) = 0;
	virtual void cast(void *raw, void *dest) = 0;

	static void add(DIWCast *cast);
	static DIWCast *find(const std::type_info &info, const DIWrapper &wrapper);
};

template <typename From, typename To>
struct DIWCastImpl final : public DIWCast {
	bool wouldCast(const std::type_info &info) override;
	bool isSame(const DIWrapper &wrapper) override;
	void cast(void *raw, void *dest) override;

	static_assert(
		std::is_base_of<To, From>::value,
		"Invalid cast, there is no inheritance defined"
	);

};

/**
 * Interface for hook calling implementations.
 */
struct DIWHook : public DIWMethodHelper {
	virtual ~DIWHook();

	std::string id() const override
	{
		return "hook";
	}

	virtual void call(DIWrapper &b) = 0;
};

/**
 * Call a hook with no arguments.
 */
template <typename T, typename B>
class DIWHookHandler final : public DIWHook {
public:
	typedef void (B::*Hook)();

	DIWHookHandler(Hook hook):
		m_hook(hook)
	{
	}

	void call(DIWrapper &b) override;

private:
	Hook m_hook;
};

/**
 * Interface to wrapper around a target instance. This wrapper can be
 * used to manage a collection of polymorphic wrappers around different
 * objects. All wrappers are expected to manage the underlying instance
 * via Poco::SharedPtr<?>.
 */
class DIWrapper {
	friend DependencyInjector;
public:
	virtual ~DIWrapper();

	/**
	 * Return pointer to Poco::SharedPtr<?>
	 */
	virtual void *raw() const = 0;

	/**
	 * Return count of references in the Poco::SharedPtr<?>
	 */
	virtual int referenceCount() = 0;

	/**
	 * Return type of the underlying instance.
	 * This is typeid(?) from Poco::SharedPtr<?>.
	 */
	virtual const std::type_info &type() const = 0;

protected:
	virtual void injectRef(const std::string &name,
			DIWrapper &wrapper) = 0;
	virtual void injectNumber(const std::string &name,
			double value) = 0;
	virtual void injectText(const std::string &name,
			const std::string &value) = 0;
	virtual void injectTime(const std::string &name,
			const Poco::Timespan &value) = 0;
	virtual void injectList(const std::string &name,
			const std::list<Poco::Dynamic::Var> &l) = 0;
	virtual void injectMap(const std::string &name,
			const std::map<Poco::Dynamic::Var, Poco::Dynamic::Var> &l) = 0;
	virtual void callHook(const std::string &name) = 0;
	virtual bool hasHook(const std::string &name) const = 0;
};

/**
 * Abstract implementation of wrapper. It holds the underlying
 * instance as Poco::SharedPtr<T>.
 */
template <typename T>
class AbstractDIWrapper : public DIWrapper {
	friend DependencyInjector;
public:
	AbstractDIWrapper();
	virtual ~AbstractDIWrapper();

	/**
	 * Get shared pointer of the underlying instance.
	 */
	Poco::SharedPtr<T> instance();

	void *raw() const override;
	int referenceCount() override;
	const std::type_info &type() const override;

protected:
	void injectRef(const std::string &name, DIWrapper &wrapper) override;
	void injectNumber(const std::string &name, double value) override;
	void injectText(const std::string &name, const std::string &value) override;
	void injectTime(const std::string &name, const Poco::Timespan &value) override;
	void injectList(const std::string &name,
			const std::list<Poco::Dynamic::Var> &l) override;
	void injectMap(const std::string &name,
		       const std::map<Poco::Dynamic::Var, Poco::Dynamic::Var> &m) override;

	template <typename Setter, typename Target>
	void injectGeneric(const std::string &name,
			Target &value,
			const std::string &property);

	void callHook(const std::string &name) override;
	bool hasHook(const std::string &name) const override;

	template <typename B, typename I>
	void setter(const std::string &name, void (B::*setter)(I *));

	template <typename B, typename I>
	void setter(const std::string &name, void (B::*setter)(Poco::SharedPtr<I>));

	template <typename B>
	void setter(const std::string &name, void (B::*setter)(int));

	template <typename B>
	void setter(const std::string &name, void (B::*setter)(double));

	template <typename B>
	void setter(const std::string &name, void (B::*setter)(bool));

	template <typename B>
	void setter(const std::string &name, void (B::*setter)(const std::string &));

	template <typename B>
	void setter(const std::string &name, void (B::*setter)(const char));

	template <typename B>
	void setter(const std::string &name, void (B::*setter)(const Poco::Timespan &));

	template <typename B>
	void setter(const std::string &name,
		void (B::*setter)(const std::list<std::string> &));

	template <typename B>
	void setter(const std::string &name,
		void (B::*setter)(const std::map<std::string, std::string> &));

	template <typename B>
	void hookHandler(const std::string &name, void (B::*hook)());

	/**
	 * Generic install method implementation to be used by all other
	 * templated install methods. Installs the given DIWMethodHelper
	 * into the m_method map.
	 */
	void installMethod(const std::string &name, DIWMethodHelper *helper);

private:
	Poco::SharedPtr<T> m_instance;
	std::map<std::string, DIWMethodHelper *> m_method;
};

/**
 * Extract instance of the given wrapper. We expect that the underlying
 * wrapper holds an instance of T which is derived from B or T == B.
 * If the wrapper is not AbstractDIWrapper<T>, then std::bad_cast is
 * thrown.
 *
 * Usually, the method is called with *this as the DIWrapper argument
 * and thus we are sure it really is AbstractDIWrapper<T>.
 * Also, the B must be castable from T. This should be assured by
 * compile-time check during setter registration.
 */
template <typename T, typename B>
B &DIWMethodHelper::extractInstance(DIWrapper &w)
{
	static_assert(
		std::is_same<B, T>::value
		||
		(std::is_polymorphic<T>::value && std::is_base_of<B, T>::value),
		"Dynamic casting is impossible here"
	);

	AbstractDIWrapper<T> &wrapper = dynamic_cast<AbstractDIWrapper<T> &>(w);
	Poco::SharedPtr<T> instance = wrapper.instance();
	return dynamic_cast<B &>(*instance.get());
}

template <typename T, typename B>
void DIWStringSetter<T, B>::call(DIWrapper &b, const std::string &text)
{
	B &base = extractInstance<T, B>(b);
	(base.*m_setter)(text);
}

template <typename T, typename B>
void DIWCharSetter<T, B>::call(DIWrapper &b, const std::string &text)
{
	if (text.size() != 1)
		throw DIWWrongInputException("expected a single character: " + text);

	B &base = extractInstance<T, B>(b);
	(base.*m_setter)(text.at(0));
}

template <typename T, typename B>
void DIWTimespanSetter<T, B>::call(DIWrapper &b, const Poco::Timespan &time)
{
	B &base = extractInstance<T, B>(b);
	(base.*m_setter)(time);
}

template <typename T, typename B>
void DIWIntSetter<T, B>::call(DIWrapper &b, double value)
{
	B &base = extractInstance<T, B>(b);

	if (value > INT_MAX)
		throw DIWWrongInputException("too big integer value");
	if (value < INT_MIN)
		throw DIWWrongInputException("too small integer value");
	if (((double) ((int) value)) != value)
		throw DIWWrongInputException("given number is not an integer");

	(base.*m_setter)((int) value);
}

template <typename T, typename B>
void DIWDoubleSetter<T, B>::call(DIWrapper &b, double value)
{
	B &base = extractInstance<T, B>(b);
	(base.*m_setter)(value);
}

template <typename T, typename B>
void DIWBoolSetter<T, B>::call(DIWrapper &b, double value)
{
	B &base = extractInstance<T, B>(b);
	(base.*m_setter)(value != 0? true : false);
}

template <typename I>
Poco::SharedPtr<I> DIWRefSetter::extractTarget(DIWrapper &i) const
{
	DIWCast *cast = DIWCast::find(typeid(I), i);
	if (cast == NULL)
		throw DIWCastException(i.type(), typeid(I));

	Poco::SharedPtr<I> inject;
	cast->cast(i.raw(), reinterpret_cast<void *>(&inject));

	return inject;
}

template <typename T, typename B, typename I>
void DIWRawPtrSetter<T, B, I>::call(DIWrapper &b, DIWrapper &i)
{
	B &base = extractInstance<T, B>(b);
	Poco::SharedPtr<I> inject = extractTarget<I>(i);
	(base.*m_setter)(inject.get());
}

template <typename T, typename B, typename I>
void DIWSharedPtrSetter<T, B, I>::call(DIWrapper &b, DIWrapper &i)
{
	B &base = extractInstance<T, B>(b);
	Poco::SharedPtr<I> inject = extractTarget<I>(i);
	(base.*m_setter)(inject);
}

template <typename T, typename B>
void DIWStringListSetter<T, B>::call(DIWrapper &b, const std::list<Poco::Dynamic::Var> &l)
{
	std::list<std::string> value;
	for (auto &v : l)
		value.push_back(v.toString());

	B &base = extractInstance<T, B>(b);
	(base.*m_setter)(value);
}

template <typename T, typename B>
void DIWStringStringMapSetter<T, B>::call(DIWrapper &b,
			const std::map<Poco::Dynamic::Var, Poco::Dynamic::Var> &m)
{
	std::map<std::string, std::string> value;
	for (auto &pair : m)
		value.emplace(pair.first.toString(), pair.second.toString());

	B &base = extractInstance<T, B>(b);
	(base.*m_setter)(value);
}

/**
 * Test this cast can cast to the given type.
 */
template <typename From, typename To>
bool DIWCastImpl<From, To>::wouldCast(const std::type_info &info)
{
	return typeid(To) == info;
}

/**
 * Test this cast works with the same base type as the given wrapper.
 */
template <typename From, typename To>
bool DIWCastImpl<From, To>::isSame(const DIWrapper &wrapper)
{
	return wrapper.type() == typeid(From);
}

/**
 * This is he most magical thing in the universe.
 * We get two pointers we *know* they point to Poco::SharedPtr<From>
 * and to Poco::SharedPtr<To> and do the cast by assigning them.
 *
 * This code can be executed only when wouldCast() and isSame() return true.
 */
template <typename From, typename To>
void DIWCastImpl<From, To>::cast(void *raw, void *dest)
{
	Poco::SharedPtr<From> *from = reinterpret_cast<Poco::SharedPtr<From> *>(raw);
	Poco::SharedPtr<To> *to = reinterpret_cast<Poco::SharedPtr<To> *>(dest);

	*to = *from;
}

template <typename T, typename B>
void DIWHookHandler<T, B>::call(DIWrapper &b)
{
	B &base = extractInstance<T, B>(b);
	(base.*m_hook)();
}

template <typename T>
AbstractDIWrapper<T>::AbstractDIWrapper():
	m_instance(new T)
{
}

template <typename T>
AbstractDIWrapper<T>::~AbstractDIWrapper()
{
	for (auto s : m_method)
		delete s.second;
}

template <typename T>
Poco::SharedPtr<T> AbstractDIWrapper<T>::instance()
{
	return m_instance;
}

template <typename T>
void *AbstractDIWrapper<T>::raw() const
{
	auto *self = const_cast<AbstractDIWrapper<T> *>(this);
	return reinterpret_cast<void *>(&self->m_instance);
}

template <typename T>
int AbstractDIWrapper<T>::referenceCount()
{
	return m_instance.referenceCount();
}

template <typename T>
const std::type_info &AbstractDIWrapper<T>::type() const
{
	return typeid(T);
}

template <typename T>
template <typename Setter, typename Target>
void AbstractDIWrapper<T>::injectGeneric(
		const std::string &name,
		Target &value,
		const std::string &property)
{
	auto entry = m_method.find(name);
	if (entry == m_method.end()) {
		throw Poco::NotFoundException("missing "
				+ property + " property "
				+ name + " for type "
				+ typeid(T).name());
	}

	Setter *setter = dynamic_cast<Setter *>(entry->second);
	if (setter == NULL) {
		throw DIWWrongInputException(
			"injecting property "
			+ property
			+ " via method "
			+ entry->second->id());
	}

	setter->call(*this, value);
}

template <typename T>
void AbstractDIWrapper<T>::injectRef(
		const std::string &name,
		DIWrapper &wrapper)
{
	injectGeneric<DIWRefSetter>(name, wrapper, "ref");
}

template <typename T>
void AbstractDIWrapper<T>::injectNumber(
		const std::string &name,
		double value)
{
	injectGeneric<DIWNumberSetter>(name, value, "number");
}

template <typename T>
void AbstractDIWrapper<T>::injectText(
		const std::string &name,
		const std::string &value)
{
	injectGeneric<DIWTextSetter>(name, value, "text");
}

template <typename T>
void AbstractDIWrapper<T>::injectTime(
		const std::string &name,
		const Poco::Timespan &value)
{
	injectGeneric<DIWTimeSetter>(name, value, "time");
}

template <typename T>
void AbstractDIWrapper<T>::injectList(
		const std::string &name,
		const std::list<Poco::Dynamic::Var> &value)
{
	injectGeneric<DIWListSetter>(name, value, "list");
}

template <typename T>
void AbstractDIWrapper<T>::injectMap(
		const std::string &name,
		const std::map<Poco::Dynamic::Var, Poco::Dynamic::Var> &value)
{
	injectGeneric<DIWMapSetter>(name, value, "map");
}

template <typename T>
void AbstractDIWrapper<T>::callHook(const std::string &name)
{
	auto entry = m_method.find(name);
	if (entry == m_method.end())
		throw Poco::NotFoundException("no such hook " + name);

	DIWHook &handler = dynamic_cast<DIWHook &>(*(entry->second));
	handler.call(*this);
}

template <typename T>
bool AbstractDIWrapper<T>::hasHook(const std::string &name) const
{
	return m_method.find(name) != m_method.end();
}

template <typename T>
void AbstractDIWrapper<T>::installMethod(const std::string &name, DIWMethodHelper *helper)
{
	if (m_method.find(name) != m_method.end()) {
		delete helper;
		throw DIWDuplicateException(name, typeid(*helper));
	}

	m_method[name] = helper;
}

template <typename T> template <typename B, typename I>
void AbstractDIWrapper<T>::setter(
		const std::string &name,
		void (B::*setter)(I *))
{
	installMethod(name, new DIWRawPtrSetter<T, B, I>(setter));
}

template <typename T> template <typename B, typename I>
void AbstractDIWrapper<T>::setter(
		const std::string &name,
		void (B::*setter)(Poco::SharedPtr<I>))
{
	installMethod(name, new DIWSharedPtrSetter<T, B, I>(setter));
}

template <typename T> template <typename B>
void AbstractDIWrapper<T>::setter(
		const std::string &name,
		void (B::*setter)(int))
{
	installMethod(name, new DIWIntSetter<T, B>(setter));
}

template <typename T> template <typename B>
void AbstractDIWrapper<T>::setter(
		const std::string &name,
		void (B::*setter)(double))
{
	installMethod(name, new DIWDoubleSetter<T, B>(setter));
}

template <typename T> template <typename B>
void AbstractDIWrapper<T>::setter(
		const std::string &name,
		void (B::*setter)(bool))
{
	installMethod(name, new DIWBoolSetter<T, B>(setter));
}

template <typename T> template <typename B>
void AbstractDIWrapper<T>::setter(
		const std::string &name,
		void (B::*setter)(const std::string &))
{
	installMethod(name, new DIWStringSetter<T, B>(setter));
}

template <typename T> template <typename B>
void AbstractDIWrapper<T>::setter(
		const std::string &name,
		void (B::*setter)(const char))
{
	installMethod(name, new DIWCharSetter<T, B>(setter));
}

template <typename T> template <typename B>
void AbstractDIWrapper<T>::setter(
		const std::string &name,
		void (B::*setter)(const Poco::Timespan &))
{
	installMethod(name, new DIWTimespanSetter<T, B>(setter));
}

template <typename T> template <typename B>
void AbstractDIWrapper<T>::setter(
		const std::string &name,
		void (B::*setter)(const std::list<std::string> &))
{
	installMethod(name, new DIWStringListSetter<T, B>(setter));
}

template <typename T> template <typename B>
void AbstractDIWrapper<T>::setter(
		const std::string &name,
		void (B::*setter)(const std::map<std::string, std::string> &))
{
	installMethod(name, new DIWStringStringMapSetter<T, B>(setter));
}

template <typename T> template <typename B>
void AbstractDIWrapper<T>::hookHandler(
		const std::string &name,
		void (B::*hook)())
{
	installMethod(name, new DIWHookHandler<T, B>(hook));
}

/**
 * Factory to create DIWrapper instances. The class manages a global
 * registry of DIWrapperFactory instances usually created via the
 * BEEEON_OBJECT_IMPL macro.
 */
class DIWrapperFactory {
public:
	virtual DIWrapper *create() const = 0;

	static void registerFactory(
		const std::string &name, DIWrapperFactory &factory);
	static DIWrapperFactory &lookupFactory(const std::string &name);
	static void listFactories(std::list<std::string> &names);
};

#define _BEEEON_VA_EXPAND(x) x
#define _BEEEON_VA_COUNT_HELPER(_1, _2, _3, _4, _5, _6, _count, ...) _count
#define _BEEEON_VA_COUNT(...) \
	_BEEEON_VA_EXPAND(_BEEEON_VA_COUNT_HELPER(__VA_ARGS__, 6, 5, 4, 3, 2, 1))
#define _BEEEON_VA_SELECT_CAT(name, count, ...)\
	_BEEEON_VA_EXPAND(name##count(__VA_ARGS__))
#define _BEEEON_VA_SELECT_HELPER(name, count, ...) \
	_BEEEON_VA_SELECT_CAT(name, count, __VA_ARGS__)
#define _BEEEON_VA_SELECT(name, ...) \
	_BEEEON_VA_SELECT_HELPER(name, _BEEEON_VA_COUNT(__VA_ARGS__), __VA_ARGS__)

#define BEEEON_OBJECT_IMPL(name, type)                  \
class name##Factory : public BeeeOn::DIWrapperFactory { \
public:                                                 \
	name##Factory()                                 \
	{                                               \
		BeeeOn::DIWrapperFactory                \
			::registerFactory(#type, *this);\
	}                                               \
                                                        \
	BeeeOn::DIWrapper *create() const override      \
	{                                               \
		return new type##DIW;                   \
	}                                               \
};                                                      \
static name##Factory name##Factory;

#define BEEEON_WRAPPER(cls, wrapper) \
	static_assert(                                         \
		std::is_default_constructible<cls>::value,     \
		#cls " is missing a default constructor");     \
	static_assert(                                         \
		!std::is_polymorphic<cls>::value               \
		||                                             \
		std::has_virtual_destructor<cls>::value,       \
		#cls " is missing a virtual destructor");      \
	struct wrapper final : public AbstractDIWrapper<cls> { \
		friend cls;                                    \
		using Self = cls;                              \
		wrapper()                                      \
		{                                              \
			DIWCast::add(new DIWCastImpl<cls, cls>);

#define BEEEON_OBJECT_BEGIN1(cls) \
BEEEON_WRAPPER(cls, cls##DIW)

#define BEEEON_OBJECT_BEGIN2(ns1, cls) \
namespace ns1 { \
BEEEON_WRAPPER(cls, cls##DIW)

#define BEEEON_OBJECT_BEGIN3(ns1, ns2, cls) \
namespace ns1 { namespace ns2 { \
BEEEON_WRAPPER(cls, cls##DIW)

#define BEEEON_OBJECT_BEGIN4(ns1, ns2, ns3, cls) \
namespace ns1 { namespace ns2 { namespace ns3 { \
BEEEON_WRAPPER(cls, cls##DIW)

#define BEEEON_OBJECT_BEGIN(...) \
	_BEEEON_VA_SELECT(BEEEON_OBJECT_BEGIN, __VA_ARGS__)
#define BEEEON_OBJECT_CASTABLE(to) \
	static_assert(                                  \
		!std::is_same<Self, to>::value,         \
		"Redundant cast to itself for " #to);   \
	static_assert(                                  \
		std::is_base_of<to, Self>::value,       \
		"Cannot cast to " #to);                 \
	static_assert(                                  \
		std::has_virtual_destructor<to>::value, \
		#to " is missing a virtual destructor");\
	DIWCast::add(new DIWCastImpl<Self, to>);
#define BEEEON_OBJECT_PROPERTY(name, method) \
	setter(name, method);

#define BEEEON_OBJECT_REF(name, method) \
	_Pragma ("GCC warning \"'BEEEON_OBJECT_REF' macro is deprecated\"") \
	BEEEON_OBJECT_PROPERTY(name, method)
#define BEEEON_OBJECT_NUMBER(name, method) \
	_Pragma ("GCC warning \"'BEEEON_OBJECT_NUMBER' macro is deprecated\"") \
	BEEEON_OBJECT_PROPERTY(name, method)
#define BEEEON_OBJECT_TEXT(name, method) \
	_Pragma ("GCC warning \"'BEEEON_OBJECT_TEXT' macro is deprecated\"") \
	BEEEON_OBJECT_PROPERTY(name, method)
#define BEEEON_OBJECT_TIME(name, method) \
	_Pragma ("GCC warning \"'BEEEON_OBJECT_TIME' macro is deprecated\"") \
	BEEEON_OBJECT_PROPERTY(name, method)
#define BEEEON_OBJECT_LIST(name, method) \
	_Pragma ("GCC warning \"'BEEEON_OBJECT_LIST' macro is deprecated\"") \
	BEEEON_OBJECT_PROPERTY(name, method)
#define BEEEON_OBJECT_MAP(name, method) \
	_Pragma ("GCC warning \"'BEEEON_OBJECT_MAP' macro is deprecated\"") \
	BEEEON_OBJECT_PROPERTY(name, method)
#define BEEEON_OBJECT_HOOK(name, method) \
	hookHandler(name, method);

#define BEEEON_OBJECT_END1(cls) \
		}               \
	};                      \
BEEEON_OBJECT_IMPL(cls, cls)

#define BEEEON_OBJECT_END2(ns1, cls) \
		}                    \
	};                           \
}                                    \
BEEEON_OBJECT_IMPL(ns1##_##cls, ns1::cls)

#define BEEEON_OBJECT_END3(ns1, ns2, cls) \
		}                         \
	};                                \
}}                                        \
BEEEON_OBJECT_IMPL(ns1##_##ns2##_##cls, ns1::ns2::cls)

#define BEEEON_OBJECT_END4(ns1, ns2, ns3, cls) \
		}                              \
	};                                     \
}}}                                            \
BEEEON_OBJECT_IMPL(ns1##_##ns2##_##ns3##_##cls, ns1::ns2::ns3::cls)

#define BEEEON_OBJECT_END(...) \
	_BEEEON_VA_SELECT(BEEEON_OBJECT_END, __VA_ARGS__)

}
