#include <cmath>
#include <list>

#include <Poco/Exception.h>
#include <Poco/Path.h>
#include <Poco/StringTokenizer.h>
#include <Poco/Dynamic/Var.h>

#include "di/DependencyInjector.h"
#include "math/SimpleCalc.h"
#include "util/ClassInfo.h"
#include "math/LogicalExpression.h"
#include "util/MultiException.h"
#include "util/TimespanParser.h"

using namespace std;
using namespace Poco;
using namespace Poco::Dynamic;
using namespace Poco::Util;
using namespace BeeeOn;

namespace BeeeOn {

class InstanceInfo {
public:
	InstanceInfo(const string &name):
		m_name(name)
	{
	}

	const string &name() const
	{
		return m_name;
	}

	void validate(AutoPtr<AbstractConfiguration> conf) const
	{
		const string base("instance[@name='" + m_name + "']");

		if (!conf->has(base)) {
			throw NotFoundException(
				"missing instance of name " + m_name);
		}

		if (!conf->has(base + "[@class]")) {
			throw NotFoundException(
				"attribute 'class' not specified for instance "
				+ m_name);
		}
	}

	string library(AutoPtr<AbstractConfiguration> conf) const
	{
		const string lib("instance[@name='" + m_name + "'][@library]");
		return conf->getString(lib, "");
	}

	const string resolveAlias(AutoPtr<AbstractConfiguration> conf) const
	{
		const string ref("alias[@name='" + m_name + "'][@ref]");
		const string &value = conf->getString(ref, "");

		if (value.compare(m_name) == 0)
			throw IllegalStateException("alias " + m_name
					+ " refers to itself");

		return value;
	}

	const string resolveClass(AutoPtr<AbstractConfiguration> conf) const
	{
		string entry("instance[@name='" + m_name + "'][@class]");
		return conf->getString(entry);
	}

	bool testConditions(AutoPtr<AbstractConfiguration> conf,
			const string &key) const
	{
		const string if_yes(key + "[@if-yes]");

		/* no such condition */
		if (!conf->has(if_yes))
			return true;

		const string &value = conf->getString(if_yes);
		return value == "y" || value == "yes" || value == "true";
	}

	void resolveKeys(AutoPtr<AbstractConfiguration> conf,
			AbstractConfiguration::Keys &keys) const
	{
		const string base("instance[@name='" + m_name + "']");

		AbstractConfiguration::Keys tmp;
		conf->keys(base, tmp);

		AbstractConfiguration::Keys::const_iterator it;
		for (it = tmp.begin(); it != tmp.end(); ++it) {
			const string &key = *it;
			if (key.find("set") != 0 && key.find("add") != 0)
				continue;

			if (!testConditions(conf, base + "." + key))
				continue;

			keys.push_back(base + "." + key);
		}
	}

private:
	string m_name;
};

}

void DependencyInjector::destroyOne(DIWrapper *one) const
{
	logger().debug(
		"deleting an instance of "
		+ ClassInfo(one->type()).name(),
		__FILE__, __LINE__);

	delete one;
}

void DependencyInjector::cleanup(const WrapperVector vec) const
{
	for (auto wrapper : vec) {
		if (wrapper->hasHook("cleanup")) {
			try {
				logger().debug("calling hook 'cleanup' for "
					+ ClassInfo(wrapper->type()).name());

				wrapper->callHook("cleanup");
			}
			BEEEON_CATCH_CHAIN(logger())
		}
	}
}

DependencyInjector::WrapperVector DependencyInjector::tryDestroy(const WrapperVector vec) const
{
	WrapperVector alive;

	for (auto wrapper : vec) {
		const int count = wrapper->referenceCount();
		if (count > 1)
			alive.emplace_back(wrapper);
		else
			destroyOne(wrapper);
	}

	return alive;
}

void DependencyInjector::destroyRest(const WrapperVector vec) const
{
	for (auto wrapper : vec) {
		const int count = wrapper->referenceCount();
		if (count > 1) {
			logger().warning(
				"an instance of "
				+ ClassInfo(wrapper->type()).name()
				+ " is still referenced ("
				+ to_string(wrapper->referenceCount() - 1)
				+ ")",
				__FILE__, __LINE__);
		}

		destroyOne(wrapper);
	}
}

DependencyInjector::DependencyInjector(
		AutoPtr<AbstractConfiguration> conf,
		const vector<string> &paths,
		bool avoidEarly):
	m_conf(conf),
	m_librariesPaths(paths)
{
	computeConstants();

	if (!avoidEarly) {
		try {
			createEarly();
		}
		BEEEON_CATCH_CHAIN_ACTION(logger(),
			destroyAll();
			throw)
	}
}

void DependencyInjector::destroyAll()
{
	logger().debug(
		"destroying " + to_string(m_free.size()) + " instances",
		__FILE__, __LINE__);

	m_set.clear();

	WrapperVector alive = m_free;
	m_free.clear();

	cleanup(alive);

	while (!alive.empty()) {
		const size_t count = alive.size();

		logger().trace("try destroy up to " + to_string(count) + " instances",
				__FILE__, __LINE__);

		alive = tryDestroy(alive);
		if (count == alive.size()) {
			logger().warning(
				"could not delete all instances,"
				" there might be a circular dependency",
				__FILE__, __LINE__);
			break;
		}
	}

	destroyRest(alive);
}

DependencyInjector::~DependencyInjector()
{
	destroyAll();
}

void DependencyInjector::computeConstants()
{
	AbstractConfiguration::Keys tmp;
	m_conf->keys(tmp);

	AbstractConfiguration::Keys::const_iterator it;
	for (it = tmp.begin(); it != tmp.end(); ++it) {
		const string &key = *it;

		if (key.find("constant[") == string::npos && key != "constant")
			continue;

		if (!m_conf->has(key + "[@name]"))
			continue;

		const string &name = m_conf->getString(key + "[@name]");
		string value;

		logger().trace("visiting constant " + name, __FILE__, __LINE__);

		if (m_conf->has(key + "[@text]")) {
			value = m_conf->getString(key + "[@text]");

			logger().debug(
				"evaluating constant " + name + " value " + value,
				__FILE__, __LINE__);

			m_conf->setString(name, value);
		}
		else if (m_conf->has(key + "[@time]")) {
			value = m_conf->getString(key + "[@time]");
			const auto result = TimespanParser::parse(value);

			logger().debug(
				"evaluating constant " + name
				+ " value " + value
				+ " as " + to_string(result.totalMicroseconds()),
				__FILE__, __LINE__);

			m_conf->setInt64(name, result.totalMicroseconds());
		}
		else if (m_conf->has(key + "[@number]")) {
			value = m_conf->getString(key + "[@number]");
			SimpleCalc calc;
			const double result = calc.evaluate(value);

			logger().debug(
				"evaluating constant " + name
				+ " value " + value
				+ " as " + to_string(result),
				__FILE__, __LINE__);

			if (::floor(result) == result)
				m_conf->setInt64(name, static_cast<Int64>(result));
			else
				m_conf->setDouble(name, result);
		}
		else if (m_conf->has(key + "[@yes-when]")) {
			value = m_conf->getString(key + "[@yes-when]");
			const auto expr = LogicalExpression::parse(value);

			logger().debug(
				"evaluating constant " + name
				+ " value " + value
				+ " as " + to_string(expr.result()),
				__FILE__, __LINE__);

			if (expr.result())
				m_conf->setString(name, "yes");
			else
				m_conf->setString(name, "no");
		}
		else {
			throw NotFoundException("missing attribute text, time or number");
		}
	}
}

void DependencyInjector::createEarly()
{
	AbstractConfiguration::Keys tmp;
	m_conf->keys(tmp);

	AbstractConfiguration::Keys::const_iterator it;
	for (it = tmp.begin(); it != tmp.end(); ++it) {
		const string &key = *it;

		if (key.find("instance[") == string::npos && key != "instance")
			continue;

		if (!m_conf->has(key + "[@name]"))
			continue;

		const string &init = m_conf->getString(
				key + "[@init]", "lazy");
		if (init.compare("early"))
			continue;

		const string name = m_conf->getString(key + "[@name]");
		(void) createNoAlias(name);
	}
}

DIWrapper *DependencyInjector::resolveAndCreate(const string &name, bool disown)
{
	InstanceInfo info(name);
	const string &ref = info.resolveAlias(m_conf);

	if (ref.empty())
		return createNoAlias(info, disown);

	DIWrapper *existing = find(ref);
	if (existing != NULL) {
		logger().debug("instance " + name
				+ " reused as alias to " + ref,
				__FILE__, __LINE__);
		return existing;
	}

	InstanceInfo aliasInfo(ref);
	return createNoAlias(aliasInfo, disown);
}

DIWrapper *DependencyInjector::createImpl(const string &name, bool disown)
{
	DIWrapper *existing = findImpl(name);
	if (existing != NULL) {
		logger().debug("instance " + name + " reused",
				__FILE__, __LINE__);
		return existing;
	}

	return resolveAndCreate(name, disown);
}

DIWrapper *DependencyInjector::findImpl(const string &name)
{
	WrapperMap::const_iterator it = m_set.find(name);
	if (it != m_set.end())
		return it->second;

	return NULL;
}

void DependencyInjector::castImpl(
		const type_info &type,
		const DIWrapper &wrapper,
		void *target)
{
		DIWCast *cast = DIWCast::find(type, wrapper);
		if (cast == NULL)
			throw DIWCastException(wrapper.type(), type);

		cast->cast(wrapper.raw(), target);
}

DIWrapper *DependencyInjector::createNoAlias(
		const InstanceInfo &info, bool disown)
{
	DIWrapper *t = createNew(info);
	if (t == NULL)
		throw Poco::NullPointerException("failed to create target "
				+ info.name());

	if (!disown) {
		m_set.insert(make_pair(info.name(), t));
		m_free.push_back(t);
	}

	return injectDependencies(info, t);
}

void DependencyInjector::loadLibrary(SharedLibrary &library, const string &name) const
{
	MultiException ex("failed to load library " + name);
	const string libName = "lib" + name + SharedLibrary::suffix();

	const auto paths = m_librariesPaths;

	if (paths.empty())
		throw LibraryLoadException("no ldpath has been specified");

	for (const auto &dir : paths) {
		const Path path(dir, libName);

		if (logger().debug()) {
			logger().debug(
				"attempt to load library " + name
				+ " from " + path.toString(),
				__FILE__, __LINE__);
		}

		try {
			library.load(path.toString(), SharedLibrary::SHLIB_GLOBAL);
		}
		catch (const LibraryAlreadyLoadedException &) {
			break;
		}
		catch (const LibraryLoadException &e) {
			ex.caught(e);
		}
	}

	if (!ex.empty())
		throw ex;
}

DIWrapper *DependencyInjector::createNew(const InstanceInfo &info)
{
	info.validate(m_conf);
	const string lib = info.library(m_conf);

	if (!lib.empty()) {
		auto &library = m_libraries[lib];

		if (!library.isLoaded()) {
			logger().notice("loading library " + lib);
			loadLibrary(library, lib);
		}
	}

	const string cls = info.resolveClass(m_conf);

	logger().debug("creating " + info.name() + " as " + cls);

	DIWrapperFactory &factory = DIWrapperFactory::lookupFactory(cls);
	return factory.create();
}

void DependencyInjector::createAndInjectRef(
		const string &targetName,
		DIWrapper *target,
		const string &name,
		const string &value)
{
	logger().debug("injecting " + value + " as " + name
			+ " into " + targetName);

	DIWrapper *ref = NULL;

	try {
		ref = createImpl(value, false);
	} catch (const Exception &e) {
		logger().error("failed to create ref " + value,
				__FILE__, __LINE__);
		e.rethrow();
	}

	if (ref == NULL) {
		throw NullPointerException(
				"failed to create ref " + value);
	}

	target->injectRef(name, *ref);
}

bool DependencyInjector::tryInjectRef(
		const InstanceInfo &info,
		DIWrapper *target,
		const string &key,
		const string &name)
{
	if (m_conf->has(key + "[@ref]")) {
		const string value = m_conf->getString(key + "[@ref]");

		createAndInjectRef(info.name(), target, name, value);
		return true;
	}

	return false;
}

void DependencyInjector::evalAndInjectNumber(
		const string &targetName,
		DIWrapper *target,
		const string &name,
		const string &value)
{
		SimpleCalc calc;
		const double number = calc.evaluate(value);

		logger().debug("injecting number " + to_string(number)
				+ " as " + name + " into " + targetName);

		target->injectNumber(name, number);
}

bool DependencyInjector::tryInjectNumber(
		const InstanceInfo &info,
		DIWrapper *target,
		const string &key,
		const string &name)
{
	if (m_conf->has(key + "[@number]")) {
		const string &tmp = m_conf->getString(key + "[@number]");
		evalAndInjectNumber(info.name(), target, name, tmp);
		return true;
	}

	return false;
}

bool DependencyInjector::tryInjectText(
		const InstanceInfo &info,
		DIWrapper *target,
		const string &key,
		const string &name)
{
	if (m_conf->has(key + "[@text]")) {
		const string value = m_conf->getString(key + "[@text]");

		logger().debug("injecting " + value + " as " + name
				+ " into " + info.name());

		target->injectText(name, value);
		return true;
	}

	return false;
}

bool DependencyInjector::tryInjectTime(
		const InstanceInfo &info,
		DIWrapper *target,
		const string &key,
		const string &name)
{
	if (m_conf->has(key + "[@time]")) {
		const string value = m_conf->getString(key + "[@time]");

		logger().debug("injecting " + value + " as " + name
				+ " into " + info.name());

		const Timespan &span = TimespanParser::parse(value);
		target->injectTime(name, span);
		return true;
	}

	return false;
}

bool DependencyInjector::tryInjectList(
		const InstanceInfo &info,
		DIWrapper *target,
		const string &key,
		const string &name)
{
	if (m_conf->has(key + "[@list]")) {
		const string value = m_conf->getString(key + "[@list]");

		logger().debug("injecting " + value + " as " + name
				+ " into " + info.name());

		StringTokenizer tokenizer(value, ",;",
				StringTokenizer::TOK_TRIM);
		list<Var> tmp;
		for (const auto &s : tokenizer)
			tmp.push_back(s);

		target->injectList(name, tmp);
		return true;
	}

	return false;
}

bool DependencyInjector::tryInjectMap(
		const InstanceInfo &info,
		DIWrapper *target,
		const string &key,
		const string &name)
{
	if (!m_conf->has(key + ".pair") && !m_conf->has(key + ".pair[1]"))
		return false;

	AbstractConfiguration::Keys keys;
	m_conf->keys(key, keys);

	map<Var, Var> m;

	for (auto &pair : keys) {
		if (!m_conf->has(key + "." + pair + "[@key]")) {
			logger().warning("missing attribute @key for "
				+ key + "." + pair);
			continue;
		}

		if (!m_conf->has(key + "." + pair + "[@text]")) {
			logger().error("missing attribute @text for "
				+ key + "." + pair);
			return false;
		}

		const string &mapKey = m_conf->getString(
				key + "." + pair + "[@key]");
		const string &mapText = m_conf->getString(
				key + "." + pair + "[@text]");

		logger().debug("map pair " + mapKey + " -> " + mapText,
			       __FILE__, __LINE__);

		if (!m.emplace(mapKey, mapText).second) {
			throw InvalidArgumentException(
				"duplicate map key " + mapKey + " for " + key
				+ " when injecting into " + info.name()
			);
		}
	}

	logger().debug("injecting map " + name + " into " + info.name(),
			__FILE__, __LINE__);

	target->injectMap(name, m);
	return true;
}

void DependencyInjector::injectValue(
		const InstanceInfo &info,
		DIWrapper *target,
		const string &key,
		const string &name)
{
	if (tryInjectRef(info, target, key, name))
		return;

	if (tryInjectNumber(info, target, key, name))
		return;

	if (tryInjectText(info, target, key, name))
		return;

	if (tryInjectTime(info, target, key, name))
		return;

	if (tryInjectList(info, target, key, name))
		return;

	if (tryInjectMap(info, target, key, name))
		return;

	logger().error("malformed configuration entry "
			+ key + " for " + info.name(),
			__FILE__, __LINE__);
}

DIWrapper *DependencyInjector::injectDependencies(
		const InstanceInfo &info,
		DIWrapper *target)
{
	AbstractConfiguration::Keys keys;
	info.resolveKeys(m_conf, keys);

	AbstractConfiguration::Keys::const_iterator it;
	for (it = keys.begin(); it != keys.end(); ++it) {
		const string &key = *it;

		logger().trace("visiting key " + key);

		if (!m_conf->has(key + "[@name]")) {
			logger().warning("missing @name for " + key,
					__FILE__, __LINE__);
			continue;
		}

		const string name = m_conf->getString(key + "[@name]");

		try {
			injectValue(info, target, key, name);
		} catch (const Poco::Exception &e) {
			logger().error("failed inject " + name + " for "
					+ info.name(), __FILE__, __LINE__);
			e.rethrow();
		}

		logger().trace("next key after " + key);
	}

	logger().notice("successfully created " + info.name(),
			__FILE__, __LINE__);

	try {
		if (!target->hasHook("done")) {
			logger().debug("no such hook 'done' defined for "
				+ ClassInfo(target->type()).name(),
				__FILE__, __LINE__);
		}
		else {
			target->callHook("done");
		}
	} catch (const Exception &e) {
		logger().error("hook 'done' failed for " + info.name(),
				__FILE__, __LINE__);
		e.rethrow();
	}

	return target;
}
