#pragma once

#include <set>

#include <Poco/SharedPtr.h>
#include <Poco/Path.h>
#include <Poco/Logger.h>
#include <Poco/Exception.h>
#include <Poco/SingletonHolder.h>

#include "util/SAXHelper.h"
#include "util/Loggable.h"

namespace BeeeOn {

template <typename T>
struct InfoProviderComparator {
	bool operator() (const Poco::SharedPtr<T> &a,
			const Poco::SharedPtr<T> &b) const
	{
		return *a < *b;
	}
};

template <typename T>
class InfoProvider : public Loggable {
public:
	typedef std::set<Poco::SharedPtr<T>,
			InfoProviderComparator<T>> InfoSet;
	virtual ~InfoProvider();

	const Poco::SharedPtr<T> findById(const typename T::ID &id) const;

	typename InfoSet::const_iterator begin() const
	{
		return infoSet().begin();
	}

	typename InfoSet::const_iterator end() const
	{
		return infoSet().end();
	}

protected:
	bool registerInfo(const T &info);

	virtual InfoSet &infoSet();
	virtual const InfoSet &infoSet() const;

private:
	InfoSet m_infoSet;
};

template <typename T>
InfoProvider<T>::~InfoProvider()
{
}

template <typename T>
typename InfoProvider<T>::InfoSet &InfoProvider<T>::infoSet()
{
	return m_infoSet;
}

template <typename T>
const typename InfoProvider<T>::InfoSet &InfoProvider<T>::infoSet() const
{
	return m_infoSet;
}

template <typename T>
const Poco::SharedPtr<T> InfoProvider<T>::findById(const typename T::ID &id) const
{
	Poco::SharedPtr<T> pattern(new T());
	pattern->setId(id);

	auto it = infoSet().find(pattern);
	if (it == infoSet().end())
		return NULL;

	return *it;
}

template <typename T>
bool InfoProvider<T>::registerInfo(const T &info)
{
	const Poco::SharedPtr<T> copy(new T(info));

	if (infoSet().find(copy) != infoSet().end())
		return false;

	infoSet().insert(copy);
	return true;
}

template <typename T>
class NullInfoProvider : public InfoProvider<T> {
public:
	static InfoProvider<T> &instance();

protected:
	typename InfoProvider<T>::InfoSet &infoSet() override;
	const typename InfoProvider<T>::InfoSet &infoSet() const override;
};

template <typename T>
typename InfoProvider<T>::InfoSet &NullInfoProvider<T>::infoSet()
{
	throw Poco::NotImplementedException(__func__);
}

template <typename T>
const typename InfoProvider<T>::InfoSet &NullInfoProvider<T>::infoSet() const
{
	throw Poco::NotImplementedException(__func__);
}

template <typename T>
InfoProvider<T> &NullInfoProvider<T>::instance()
{
	static Poco::SingletonHolder<NullInfoProvider<T>> singleton;
	return *singleton.get();
}

template <typename T>
class XmlInfoProvider : public InfoProvider<T> {
public:
	virtual ~XmlInfoProvider();

protected:
	template <typename SAXHandler>
	void parseFile(const std::string &path,
			const std::string &infoLabel);
};

template <typename T>
XmlInfoProvider<T>::~XmlInfoProvider()
{
}

template <typename T> template <typename SAXHandler>
void XmlInfoProvider<T>::parseFile(const std::string &path,
		const std::string &infoLabel)
{
	if (path.empty())
		return;

	Poco::Path file(path);
	SAXHandler handler;

	SAXHelper::parse(file, handler);

	for (const auto &info : handler) {
		this->logger().information("register " + infoLabel
				+ " " + info.name()
				+ " with ID " + info.id().toString(),
				__FILE__, __LINE__);

		if (this->registerInfo(info))
			continue;

		this->logger().error(infoLabel + " " + info.name()
				+ " with ID " + info.id().toString()
				+ " is already registered",
				__FILE__, __LINE__);
	}
}

}
