#include "Machine.h"

Machine::Machine()
{
}

Machine::~Machine()
{
}

void Machine::Run(std::string programFile)
{
	ProgramLoader* loader = new ProgramLoader();
	Program* program = loader->Load(programFile);
	delete loader;

	if (program != NULL) {
		Environment* env = new Environment();
		Interpreter* interpreter = new Interpreter();
		interpreter->Run(program, env);

		delete interpreter;
		delete env;
		delete program;
	}
}
