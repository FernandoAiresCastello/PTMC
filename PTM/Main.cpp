#include "Machine.h"
#include "Compiler.h"
#include <TileGameLib.h>
using namespace TileGameLib;

int main(int argc, char** args)
{
	Machine* machine = new Machine();
	Compiler* compiler = new Compiler();
	auto bytecode = compiler->Compile("main.ptml");
	delete compiler;

	machine->Run(bytecode);
	delete machine;

	return 0;
}
