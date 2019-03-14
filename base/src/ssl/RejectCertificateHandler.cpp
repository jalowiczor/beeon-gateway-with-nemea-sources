#include <Poco/Logger.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/Net/CertificateHandlerFactory.h>
#include <Poco/Net/VerificationErrorArgs.h>

#include "ssl/RejectCertificateHandler.h"
#include "ssl/X509Fingerprint.h"

using namespace Poco;
using namespace Poco::Net;
using namespace BeeeOn;

BetterRejectCertificateHandler::BetterRejectCertificateHandler(bool server):
	InvalidCertificateHandler(server)
{
}

void BetterRejectCertificateHandler::onInvalidCertificate(
		const void *,
		VerificationErrorArgs &error)
{
	const X509Certificate &cert = error.certificate();

	error.setIgnoreError(false);

	logger().warning(
		"rejecting certificate:\n"
		+ format("Name: %s\n"
#if POCO_VERSION >= 0x01090000
			"Serial: %s\n"
#endif
			"Issuer: %s\n"
			"Subject: %s\n"
			"Valid from: %s\n"
			"Expires on: %s\n"
			"SHA256: %s\n"
			"due to: %s",
			cert.commonName(),
#if POCO_VERSION >= 0x01090000
			cert.serialNumber(),
#endif
			cert.issuerName(),
			cert.subjectName(),
			DateTimeFormatter::format(cert.validFrom(),
				DateTimeFormat::SORTABLE_FORMAT),
			DateTimeFormatter::format(cert.expiresOn(),
				DateTimeFormat::SORTABLE_FORMAT),
		X509Fingerprint(&cert).digestToHex("SHA256"),
		error.errorMessage()),
		__FILE__, __LINE__);
}

POCO_REGISTER_CHFACTORY(BeeeOn, BetterRejectCertificateHandler)
