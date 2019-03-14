#pragma once

#include <iostream>
#include <vector>
#include <string>

#include <Poco/SharedPtr.h>
#include <Poco/Dynamic/Var.h>

namespace BeeeOn {

class Locale;

class Translator {
public:
	typedef Poco::SharedPtr<Translator> Ptr;

	virtual ~Translator();

	/**
	 * Format the message identified by given key. This method should be
	 * used only for translations where it is sure that the translation
	 * exists otherwise a messy string could be returned easily.
	 * The method calls formatImpl internally.
	 */
	template <typename... Rest>
	std::string formatSure(
		const std::string &key,
		const Rest & ... rest)
	{
		std::vector<Poco::Dynamic::Var> tmp;
		formatCollect(tmp, rest...);
		const auto &result = formatImpl(key, tmp);
		return result.empty()? key : result;
	}

	/**
	 * Format the message identified by given key. If the key is not
	 * found, it returns the given def formatted via formatFallback().
	 * The method calls formatImpl internally.
	 */
	template <typename... Rest>
	std::string format(
		const std::string &key,
		const std::string &def,
		const Rest & ... rest)
	{
		std::vector<Poco::Dynamic::Var> tmp;
		formatCollect(tmp, rest...);
		const auto &result = formatImpl(key, tmp);
		return result.empty() ?
			formatFallback(def, tmp) : result;
	}

	/**
	 * Format the message identified by given key. If the key is not
	 * found, it returns the given def formatted via formatFallback().
	 * The method calls formatImpl internally.
	 */
	std::string vformat(
		const std::string &key,
		const std::string &def,
		const std::vector<Poco::Dynamic::Var> &args)
	{
		const auto &result = formatImpl(key, args);
		return result.empty() ?
			formatFallback(def, args) : result;
	}

protected:
	void prepareArg(std::vector<Poco::Dynamic::Var> &tmp, const char *first)
	{
		tmp.push_back(std::string(first));
	}

	template <typename Type>
	void prepareArg(std::vector<Poco::Dynamic::Var> &tmp, const Type &value)
	{
		tmp.push_back(value);
	}

	void formatCollect(std::vector<Poco::Dynamic::Var> &)
	{
	}

	template <typename First, typename... Rest>
	void formatCollect(std::vector<Poco::Dynamic::Var> &tmp,
		const First &first,
		const Rest & ... rest)
	{
		prepareArg(tmp, first);
		formatCollect(tmp, rest...);
	}

	/**
	 * Implements the actual key lookup and formatting.
	 */
	virtual std::string formatImpl(
		const std::string &key,
		const std::vector<Poco::Dynamic::Var> &args) = 0;

	/**
	 * It uses the def as a fallback format. The fallback assumes using
	 * of the Poco::format() where the Poco::Dynamic::Var is converted
	 * to Poco::Any on fly.
	 */
	std::string formatFallback(
		const std::string &def,
		const std::vector<Poco::Dynamic::Var> &args) const;
};

class TranslatorFactory {
public:
	typedef Poco::SharedPtr<TranslatorFactory> Ptr;

	virtual ~TranslatorFactory();

	/**
	 * Create an instance of Translator for the given locale
	 * and package name.
	 */
	virtual Translator *create(
			const Locale &locale,
			const std::string &name = "") = 0;
};

}
