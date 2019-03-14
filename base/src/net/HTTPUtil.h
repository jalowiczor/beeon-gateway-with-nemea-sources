#pragma once

#include <string>

#include <Poco/Logger.h>
#include <Poco/SharedPtr.h>
#include <Poco/Timespan.h>
#include <Poco/URI.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/SocketAddress.h>

#include "net/HTTPEntireResponse.h"
#include "ssl/SSLClient.h"

namespace BeeeOn {

class HTTPUtil {
public:
	HTTPUtil() = delete;
	HTTPUtil(const HTTPUtil &) = delete;
	HTTPUtil(const HTTPUtil &&) = delete;
	~HTTPUtil() = delete;

	/**
	 * @brief Sends HTTP request to target defined by URI.
	 * It also sets URI path to the request. If the timeout
	 * is negative, it is not set.
	 */
	static HTTPEntireResponse makeRequest(
		Poco::Net::HTTPRequest& request,
		const Poco::URI& uri,
		const std::string& msg,
		const Poco::Timespan& timeout = -1);

	/**
	 * @brief Sends HTTP request over SSL to target defined
	 * by URI. It also sets URI path to the request. If the
	 * timeout is negative, it is not set.
	 */
	static HTTPEntireResponse makeRequest(
		Poco::Net::HTTPRequest& request,
		const Poco::URI& uri,
		const std::string& msg,
		SSLClient::Ptr sslConfig,
		const Poco::Timespan& timeout = -1);

	/**
	 * @brief Sends HTTP request to target defined by
	 * host and port. If the timeout is negative,
	 * it is not set.
	 */
	static HTTPEntireResponse makeRequest(
		Poco::Net::HTTPRequest& request,
		const std::string& host,
		const uint16_t port,
		const std::string& msg,
		const Poco::Timespan& timeout = -1);

	/**
	 * @brief Sends HTTP request over SSL to target defined
	 * by host and port. If the timeout is negative, it is
	 * not set.
	 */
	static HTTPEntireResponse makeRequest(
		Poco::Net::HTTPRequest& request,
		const std::string& host,
		const uint16_t port,
		const std::string& msg,
		SSLClient::Ptr sslConfig,
		const Poco::Timespan& timeout = -1);

private:
	static HTTPEntireResponse sendRequest(
		Poco::SharedPtr<Poco::Net::HTTPClientSession> session,
		Poco::Net::HTTPRequest& request,
		const std::string& msg);

	static Poco::Logger &logger();
};

}
