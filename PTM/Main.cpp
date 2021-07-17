#include <SDL.h>
#include "Program.h"
#include "Machine.h"
#include "Datafile.h"

int main(int argc, char** argv)
{
	const int w = 256;
	const int h = 192;
	const int zoom = 3;

	Graphics* gr = new Graphics(w, h, zoom * w, zoom * h, false);

	Program* prog = new Program();
	prog->Load("test.ptm");
	Datafile* data = new Datafile();
	data->SetProgram(prog);
	Machine* machine = new Machine();
	machine->Run(data, gr);

	delete machine;
	delete data;
	delete gr;

	return 0;
}
