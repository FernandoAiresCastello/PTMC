#include "Debugger.h"

Debugger::Debugger(Datafile* data, Graphics* gr)
{
	DataFile = data;
	Gr = gr;
	Running = false;
}

Debugger::~Debugger()
{
}

void Debugger::Run()
{
	Running = true;
	while (Running) {
		// todo
	}
}
