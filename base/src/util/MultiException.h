#pragma once

#include <vector>

#include <Poco/Exception.h>

namespace BeeeOn {

/**
 * @brief MultiException is used to collect multiple exceptions
 * thrown during some processing we do not want to interrupt.
 * On every exception that occures during such processing, we
 * add it into a prepared MultiException instance and after the
 * processing finishes, we throw all the exceptions at once
 * via the MultiException.
 */
class MultiException : public Poco::Exception {
public:
	MultiException();

	/**
	 * @brief Construct an instance of the MultiException
	 * with a custom initial message.
	 */
	MultiException(const std::string &msg);

	/**
	 * @brief Construct an instance of the MultiException from
	 * an already collected vector of exceptions.
	 */
	MultiException(const std::vector<Poco::Exception> &e);

	MultiException(const MultiException &e);
	~MultiException();

	MultiException &operator = (const MultiException &e);
	const char *name() const noexcept override;
	Poco::Exception *clone() const override;
	void rethrow() const override;

	/**
	 * @brief Add a caught exception for later processing.
	 * It also constructs the message to contain a limited
	 * number of messages from those exceptions. To see all
	 * exceptions, it is necessary to iterate over the
	 * MultiException instance.
	 */
	void caught(const Poco::Exception &e);
	std::vector<Poco::Exception *>::const_iterator begin() const;
	std::vector<Poco::Exception *>::const_iterator end() const;

	/**
	 * @return false when there are no exceptions registerd (so there
	 * was no fail during processing and MultiException can be dropped).
	 */
	bool empty() const;

	/**
	 * @return count of caught exceptions.
	 */
	size_t count() const;

protected:
	void add(const Poco::Exception &e);
	void clear();

private:
	std::vector<Poco::Exception *> m_exceptions;
};

}
