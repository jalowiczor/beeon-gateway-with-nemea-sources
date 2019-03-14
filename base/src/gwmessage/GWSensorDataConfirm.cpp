#include <string>

#include "gwmessage/GWSensorDataConfirm.h"

using namespace std;
using namespace Poco;
using namespace BeeeOn;

GWSensorDataConfirm::GWSensorDataConfirm():
	GWMessage(GWMessageType::SENSOR_DATA_CONFIRM)
{
}

GWSensorDataConfirm::GWSensorDataConfirm(const JSON::Object::Ptr object):
	GWMessage(object)
{
}
