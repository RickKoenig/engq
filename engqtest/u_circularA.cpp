#include <m_eng.h>

#include "u_circularA.h"
#include "u_circularB.h"

void CircularA::printBoth()
{
	logger("printBoth A\n");
	logger("self A\n");
	print();
	logger("other B\n");
	cb->print();
}
