#pragma once

#include <Poco/Exception.h>
#include <Poco/Logger.h>
#include <Poco/Random.h>

#include <map>
#include <string>

namespace BeeeOn {

/**
 * Provide enum-specific helper data types.
 */
template <typename Raw>
struct EnumHelper {
	/**
	 * ValueMap that each Enum must specify.
	 */
	typedef std::map<Raw, std::string> ValueMap;
	/**
	 * A representation of Enum value. It points to a ValueMap entry
	 * to access both the Raw value and its name.
	 */
	typedef typename ValueMap::const_iterator Value;
	/**
	 * RawMap to lookup Enum instance by Raw value.
	 */
	typedef std::map<Raw, Value> RawMap;
	/**
	 * NamesMap to lookup Enum instance by name.
	 */
	typedef std::map<std::string, Value> NamesMap;

	static RawMap initRawMap(const ValueMap &valueMap)
	{
		RawMap rawMap;

		for (Value it = valueMap.begin(); it != valueMap.end(); ++it) {
			auto result = rawMap.insert(std::make_pair(it->first, it));
			if (!result.second)
				throw Poco::ExistsException(
					"duplicate raw value " + std::to_string(it->first));
		}

		return rawMap;
	}
};

template <typename Raw>
struct EnumNamesInitializer {
	typedef typename EnumHelper<Raw>::Value Value;
	typedef typename EnumHelper<Raw>::ValueMap ValueMap;
	typedef typename EnumHelper<Raw>::NamesMap NamesMap;

	EnumNamesInitializer(const NamesMap &map):
		namesMap(map)
	{
	}

	EnumNamesInitializer(const ValueMap &valueMap):
		namesMap(initNamesMap(valueMap))
	{
	}

	static NamesMap initNamesMap(const ValueMap &valueMap)
	{
		NamesMap namesMap;

		for (Value it = valueMap.begin(); it != valueMap.end(); ++it) {
			auto result = namesMap.insert(std::make_pair(it->second, it));
			if (!result.second)
				throw Poco::ExistsException(
					"duplicate string value " + it->second);
		}

		return namesMap;
	}

	const NamesMap namesMap;
};

/**
 * Abstract Enum type to simplify defining "clever" enums.
 * A "clever" enum can be deserialized (parsed) from string
 * and serialized back (toString).
 *
 * Base is the user defined class containing an embedded enum
 * called Raw. Example:
 *
 *   class TestEnum {
 *   public:
 *      enum Raw {
 *         TEST_X0,
 *         TEST_X1,
 *         ...
 *      }
 *   };
 *
 *   typedef Enum<TestEnum> Test;
 *
 * Use Test class as the target enum's type.
 */
template <typename Base, typename RawType = typename Base::Raw,
	typename NamesMapInitializer = EnumNamesInitializer<RawType>>
class Enum : public Base {
public:
	typedef RawType Raw;
	typedef Enum<Base, RawType, NamesMapInitializer> ThisEnum;

	typedef typename EnumHelper<Raw>::ValueMap ValueMap;
	typedef typename EnumHelper<Raw>::Value Value;

	Enum(const Raw &raw):
		Enum(fromRaw(raw).m_value)
	{
	}

private:
	Enum(const Value &value):
		m_value(value)
	{
		if (value == Base::valueMap().end()) {
			if (Base::valueMap().empty()) {
				throw Poco::IllegalStateException(
					"attempt to create enum that is empty");
			}

			// this is certainly a bug in the Enum class
			throw Poco::IllegalStateException(
				"invalid enum value determined");
		}
	}

	/**
	 * @brief Iterator over the enum values.
	 */
	class Iterator {
	public:
		Iterator(typename EnumHelper<Raw>::RawMap::const_iterator current):
			m_current(current)
		{
		}

		Iterator &operator ++()
		{
			++m_current;
			return *this;
		}

		Iterator operator ++(int)
		{
			return Iterator(m_current++);
		}

		bool operator ==(const Iterator &it) const
		{
			return m_current == it.m_current;
		}

		bool operator !=(const Iterator &it) const
		{
			return m_current != it.m_current;
		}

		ThisEnum operator *()
		{
			return ThisEnum(m_current->second);
		}

	private:
		typename EnumHelper<Raw>::RawMap::const_iterator m_current;
	};

	/**
	 * @brief Instantiable object that makes the enum class to be iterable.
	 * The class preserve no state and thus there can be only one per enum
	 * type (true singleton).
	 */
	class Iterable {
	public:
		Iterator begin() const
		{
			return Iterator(rawMap().begin());
		}

		Iterator end() const
		{
			return Iterator(rawMap().end());
		}
	};

protected:
	static typename EnumHelper<Raw>::RawMap &rawMap()
	{
		static typename EnumHelper<Raw>::RawMap rawMap(
			EnumHelper<Raw>::initRawMap(Base::valueMap())
		);

		return rawMap;
	}

	static const typename EnumHelper<Raw>::NamesMap &namesMap()
	{
		static NamesMapInitializer initializer(Base::valueMap());
		return initializer.namesMap;
	}

public:
	Raw raw() const
	{
		return m_value->first;
	}

	const std::string &toString() const
	{
		return m_value->second;
	}

	static const Iterable &all()
	{
		static const Iterable it;
		return it;
	}

	static ThisEnum parse(const std::string &input)
	{
		auto it = namesMap().find(input);
		if (it == namesMap().end()) {
			throw Poco::InvalidArgumentException(
				"failed to parse '" + input + "'");
		}

		return ThisEnum(it->second);
	}

	static ThisEnum fromRaw(const Raw &raw)
	{
		auto it = rawMap().find(raw);
		if (it == rawMap().end()) {
			throw Poco::InvalidArgumentException(
				"unrecognized raw value " + std::to_string(raw));
		}

		return ThisEnum(it->second);
	}

	static ThisEnum random()
	{
		Poco::Random rnd;
		rnd.seed();

		const std::size_t index = rnd.next(Base::valueMap().size());
		Value it = Base::valueMap().begin();

		for (std::size_t k = 0; it != Base::valueMap().end(); ++it, ++k) {
			if (k == index)
				break;
		}

		return ThisEnum(it);
	}

	static ThisEnum fromRaw(const unsigned int raw)
	{
		return fromRaw(Raw(raw));
	}

	operator Raw() const
	{
		return raw();
	}

	bool operator ==(const Enum &other) const
	{
		return m_value == other.m_value;
	}

	bool operator ==(const Enum::Raw &other) const
	{
		return m_value->first == other;
	}

	bool operator !=(const Enum &other) const
	{
		return m_value != other.m_value;
	}

	bool operator !=(const Enum::Raw &other) const
	{
		return m_value->first != other;
	}

	bool operator <(const Enum &other) const
	{
		return m_value->first < other.m_value->first;
	}

	bool operator <(const Enum::Raw &other) const
	{
		return m_value->first < other;
	}

	bool operator >(const Enum &other) const
	{
		return m_value->first > other.m_value->first;
	}

	bool operator >(const Enum::Raw &other) const
	{
		return m_value->first > other;
	}

	bool operator <=(const Enum &other) const
	{
		return m_value->first <= other.m_value->first;
	}

	bool operator <=(const Enum::Raw &other) const
	{
		return m_value->first <= other;
	}

	bool operator >=(const Enum &other) const
	{
		return m_value->first >= other.m_value->first;
	}

	bool operator >=(const Enum::Raw &other) const
	{
		return m_value->first >= other;
	}

private:
	Value m_value;
};

template <typename Base, typename Raw = typename Base::Raw,
	typename NamesMapInitializer = EnumNamesInitializer<Raw>>
inline std::string operator +(const std::string &s, const Enum<Base, Raw, NamesMapInitializer> &e)
{
	return s + e.toString();
}

template <typename Base, typename Raw = typename Base::Raw,
	typename NamesMapInitializer = EnumNamesInitializer<Raw>>
inline std::string operator +(const char *s, const Enum<Base, Raw, NamesMapInitializer> &e)
{
	return s + e.toString();
}

}
