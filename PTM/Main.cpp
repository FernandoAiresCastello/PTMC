#include <SDL.h>
#include "Program.h"
#include "Machine.h"
#include "Datafile.h"

int main(int argc, char** argv)
{
	Program* prog = new Program();
	prog->Load("test.ptm");
	Datafile* data = new Datafile();
	data->SetProgram(prog);
	Machine* machine = new Machine();
	machine->Run(data);

	delete machine;
	delete data;

	return 0;
}
