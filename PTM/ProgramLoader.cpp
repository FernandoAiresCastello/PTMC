#include <TBRLGPT.h>
#include "ProgramLoader.h"
#include "Util.h"
#include "Program.h"

using namespace TBRLGPT;

ProgramLoader::ProgramLoader()
{
}

ProgramLoader::~ProgramLoader()
{
}

Program* ProgramLoader::Load(std::string filename)
{
	Program* program = new Program();
	std::vector<std::string> rawProgramLines = File::ReadLines(filename);
	program->Parse(rawProgramLines);

	std::string validation = program->Validate();
	if (!validation.empty()) {
		ShowErrorMessageBox(validation);
		delete program;
		program = NULL;
	}

	return program;
}
