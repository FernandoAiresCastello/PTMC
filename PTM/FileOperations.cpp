#include "FileOperations.h"
#include "Program.h"
#include "ProgramEditor.h"
#include "SharedScreen.h"

void SaveEverything(std::string filename)
{
	std::string fileContents = "";
	Program* prog = GetProgram();
	for (int i = 0; i < prog->Lines.size(); i++) {
		fileContents.append(prog->Lines[i]->Command);
		fileContents.append("\n");
	}
	File::WriteText(filename, fileContents);
}

void LoadEverything(std::string filename)
{
	if (File::Exists(filename)) {
		auto lines = File::ReadLines(filename);
		SetProgramLines(lines);
	}
	else {
		PrintLine("File not found");
	}
}
