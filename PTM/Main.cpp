#include "Machine.h"
#include "ProgramEditor.h"

int main(int argc, char** args)
{
	Machine* machine = new Machine();
	machine->LoadProgram("main.prg");
	machine->Run();
	delete machine;

	//ProgramEditor* editor = new ProgramEditor();
	//editor->Run();
	//delete editor;

	return 0;
}
