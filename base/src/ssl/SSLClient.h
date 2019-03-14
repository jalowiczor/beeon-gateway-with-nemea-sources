#pragma once

#include <Poco/SharedPtr.h>
#include <Poco/Net/InvalidCertificateHandler.h>

#include "ssl/SSLFacility.h"

namespace BeeeOn {

/**
 * SSL Client context provider. Each group of SSL connections
 * (connection to certain server, connections by certain L7
 * protocol, etc.) would usually use an instance of the SSLClient.
 * Some applications may share a single instance.
 */
class SSLClient : public SSLFacility {
public:
	typedef Poco::SharedPtr<SSLClient> Ptr;

	SSLClient();
	~SSLClient();

protected:
	Poco::Net::Context::Ptr createContext() override;

protected:
	Poco::SharedPtr<Poco::Net::InvalidCertificateHandler>
		m_certHandler;
};

}
