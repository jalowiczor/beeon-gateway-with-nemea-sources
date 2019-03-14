#include <Poco/Logger.h>
#include <Poco/String.h>
#include <Poco/StringTokenizer.h>
#include <Poco/NumberParser.h>
#include <Poco/Net/Utility.h>
#include <Poco/Net/SSLManager.h>

#include "di/Injectable.h"
#include "ssl/SSLServer.h"
#include "ssl/RejectCertificateHandler.h"
#include "util/CryptoParams.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;
using namespace BeeeOn;

SSLServer::SSLServer():
	m_certHandler(new BetterRejectCertificateHandler(true)),
	m_preferServerCiphers(false)
{
}

SSLServer::~SSLServer()
{
}

void SSLServer::setSessionID(const string &id)
{
	m_sessionID = id;
}

void SSLServer::setPreferServerCiphers(bool prefer)
{
	m_preferServerCiphers = prefer;
}

Context::Ptr SSLServer::createContext()
{
	Context::Ptr context = new Context(
		Context::SERVER_USE,
		m_privateKey,
		m_certificate,
		m_caLocation,
		m_verificationMode,
		m_verificationDepth,
		m_loadDefaultCA,
		m_cipherList
	);

	if (trim(m_sessionID).empty()) {
		logger().notice("SSL session ID not set, generating a random one",
				__FILE__, __LINE__);

		m_sessionID = CryptoParams::randomString(16);
	}

	context->enableSessionCache(m_sessionCache, m_sessionID);

	if (m_preferServerCiphers) {
		logger().notice("prefering server ciphers over the client ones",
				__FILE__, __LINE__);

		context->preferServerCiphers();
	}

	return context;
}

BEEEON_OBJECT_BEGIN(BeeeOn, SSLServer)
BEEEON_OBJECT_PROPERTY("caLocation", &SSLFacility::setCALocation)
BEEEON_OBJECT_PROPERTY("loadDefaultCA", &SSLFacility::setLoadDefaultCA)
BEEEON_OBJECT_PROPERTY("privateKey", &SSLFacility::setPrivateKey)
BEEEON_OBJECT_PROPERTY("passphrase", &SSLFacility::setPassphrase)
BEEEON_OBJECT_PROPERTY("certificate", &SSLFacility::setCertificate)
BEEEON_OBJECT_PROPERTY("verificationMode", &SSLFacility::setVerificationMode)
BEEEON_OBJECT_PROPERTY("verificationDepth", &SSLFacility::setVerificationDepth)
BEEEON_OBJECT_PROPERTY("cipherList", &SSLFacility::setCipherList)
BEEEON_OBJECT_PROPERTY("sessionCache", &SSLFacility::setSessionCache)
BEEEON_OBJECT_PROPERTY("sessionID", &SSLServer::setSessionID)
BEEEON_OBJECT_PROPERTY("disabledProtocols", &SSLFacility::setDisabledProtocols)
BEEEON_OBJECT_PROPERTY("preferServerCiphers", &SSLServer::setPreferServerCiphers)
BEEEON_OBJECT_PROPERTY("extendedCertificateVerification", &SSLFacility::setExtendedCertificateVerification)
BEEEON_OBJECT_HOOK("done", &SSLFacility::initContext)
BEEEON_OBJECT_END(BeeeOn, SSLServer)
