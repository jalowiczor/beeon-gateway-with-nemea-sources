#include <typeinfo>

#include "util/Exception.h"

namespace BeeeOn {

POCO_IMPLEMENT_EXCEPTION(InProgressException, Poco::LogicException, "Already in progress")

}
