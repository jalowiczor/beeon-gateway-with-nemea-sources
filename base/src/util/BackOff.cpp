#include "util/BackOff.h"

using namespace BeeeOn;

const Poco::Timespan BackOff::STOP = -1;

BackOff::~BackOff()
{
}

BackOffFactory::~BackOffFactory()
{
}
