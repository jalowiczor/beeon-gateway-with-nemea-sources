#include "gwmessage/GWLastValueResponse.h"
#include "util/JsonUtil.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

GWLastValueResponse::GWLastValueResponse():
	GWResponse(GWMessageType::LAST_VALUE_RESPONSE)
{
}

GWLastValueResponse::GWLastValueResponse(const JSON::Object::Ptr object) :
	GWResponse(object)
{
}

void GWLastValueResponse::setValue(const double value)
{
	if (std::isnan(value))
		json()->set("value", Dynamic::Var());
	else
		json()->set("value", value);
}

double GWLastValueResponse::value() const
{
	return json()->getValue<double>("value");
}

void GWLastValueResponse::setValid(const bool valid)
{
	json()->set("valid", valid);
}

bool GWLastValueResponse::valid() const
{
	return json()->getValue<bool>("valid");
}
