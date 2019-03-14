#pragma once

#include <Poco/SharedPtr.h>
#include <Poco/Net/InvalidCertificateHandler.h>

#include "ssl/SSLFacility.h"

namespace BeeeOn {

class SSLServer : public SSLFacility {
public:
	typedef Poco::SharedPtr<SSLServer> Ptr;

	SSLServer();
	~SSLServer();

	void setSessionID(const std::string &id);
	void setPreferServerCiphers(bool prefer);

protected:
	Poco::Net::Context::Ptr createContext() override;

protected:
	Poco::SharedPtr<Poco::Net::InvalidCertificateHandler>
		m_certHandler;
	std::string m_sessionID;
	bool m_preferServerCiphers;
};

}
