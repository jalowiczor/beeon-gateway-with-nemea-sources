#include <iostream>

#include "di/Injectable.h"
#include "io/StdConsole.h"

BEEEON_OBJECT_BEGIN(BeeeOn, StdConsole)
BEEEON_OBJECT_CASTABLE(Console)
BEEEON_OBJECT_PROPERTY("eol", &StdConsole::setEol)
BEEEON_OBJECT_PROPERTY("skipEol", &StdConsole::setSkipEol)
BEEEON_OBJECT_PROPERTY("prompt", &StdConsole::setPrompt)
BEEEON_OBJECT_END(BeeeOn, StdConsole)

using namespace std;
using namespace BeeeOn;

StdConsole::StdConsole():
	StreamConsole(cin, cout)
{
}
