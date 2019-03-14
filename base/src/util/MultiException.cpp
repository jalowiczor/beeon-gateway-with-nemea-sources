#include "util/MultiException.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

static OutOfMemoryException EMERGENCY_EXCEPTION("MultiException failed to alloc memory");

MultiException::MultiException():
	Exception("caught multiple exceptions")
{
}

MultiException::MultiException(const string &msg):
	Exception(msg)
{
}

MultiException::MultiException(const std::vector<Poco::Exception> &e):
	MultiException()
{
	for (const auto &one : e)
		caught(one);
}

MultiException::MultiException(const MultiException &e):
	Exception(e)
{
	try {
		for (const auto &one : e)
			add(*one);
	}
	catch (...) {
		clear();
	}
}

MultiException::~MultiException()
{
	clear();
}

bool MultiException::empty() const
{
	return m_exceptions.empty();
}

size_t MultiException::count() const
{
	return m_exceptions.size();
}

void MultiException::clear()
{
	for (auto it = m_exceptions.begin(); it != m_exceptions.end(); ++it) {
		Exception *e = *it;

		if (static_cast<OutOfMemoryException *>(e) == &EMERGENCY_EXCEPTION)
			continue;

		delete e;
	}

	m_exceptions.clear();
}

MultiException &MultiException::operator = (const MultiException &e)
{
	Exception::operator =(e);
	m_exceptions = e.m_exceptions;
	return *this;
}

const char *MultiException::name() const noexcept
{
	return "MultiException";
}

Poco::Exception *MultiException::clone() const
{
	return new MultiException(*this);
}

void MultiException::rethrow() const
{
	if (m_exceptions.size() == 1)
		throw *m_exceptions[0];

	throw *this;
}

void MultiException::caught(const Poco::Exception &e)
{
	if (m_exceptions.empty())
		extendedMessage("\n" + e.displayText());
	else if (m_exceptions.size() < 3)
		message(message() + "\n" + e.displayText());
	else if (m_exceptions.size() == 3)
		message(message() + "\n...and more");

	add(e);
}

void MultiException::add(const Poco::Exception &e)
{
	try {
		m_exceptions.emplace_back(e.clone());
	}
	catch (...) {
		m_exceptions.emplace_back(&EMERGENCY_EXCEPTION);
	}
}

vector<Exception *>::const_iterator MultiException::begin() const
{
	return m_exceptions.begin();
}

vector<Exception *>::const_iterator MultiException::end() const
{
	return m_exceptions.end();
}
