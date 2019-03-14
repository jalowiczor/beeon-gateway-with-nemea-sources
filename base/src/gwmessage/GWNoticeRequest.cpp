#include "gwmessage/GWNoticeRequest.h"

using namespace std;
using namespace Poco;
using namespace Poco::JSON;
using namespace BeeeOn;

GWNoticeRequest::GWNoticeRequest():
	GWRequest(GWMessageType::NOTICE_REQUEST)
{
}

GWNoticeRequest::GWNoticeRequest(const Object::Ptr object):
	GWRequest(object)
{
}

void GWNoticeRequest::setAt(const Timestamp &at)
{
	json()->set("at", at.epochMicroseconds());
}

Timestamp GWNoticeRequest::at() const
{
	return json()->getValue<uint64_t>("at");
}

void GWNoticeRequest::setSeverity(const Severity &severity)
{
	json()->set("severity", severity.toString());
}

Severity GWNoticeRequest::severity() const
{
	return Severity::parse(json()->getValue<string>("severity"));
}

void GWNoticeRequest::setKey(const string &key)
{
	json()->set("key", key);
}

string GWNoticeRequest::key() const
{
	return json()->getValue<string>("key");
}

void GWNoticeRequest::setContext(const Object::Ptr context)
{
	json()->set("context", context);
}

Object::Ptr GWNoticeRequest::context() const
{
	if (json()->has("context"))
		return json()->getObject("context");

	return {};
}
