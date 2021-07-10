#include "FileOperations.h"
#include "Program.h"
#include "ProgramEditor.h"
#include "SharedScreen.h"
#include "SharedObjects.h"

#define DEFAULT_WORKING_DIR "./mnt/";
std::string WorkingDir = DEFAULT_WORKING_DIR;

#define ERR_FILE_NOT_FOUND "File not found"

void CreateWorkingDir()
{
	if (!File::Exists(WorkingDir)) {
		File::CreateFolder(WorkingDir);
	}
}

std::string GetWorkingDir()
{
	return WorkingDir;
}

void DeleteFile(std::string filename)
{
	std::string path = GetWorkingDir() + filename;

	if (File::Exists(path)) {
		File::Delete(path);
	}
	else {
		PrintLine(ERR_FILE_NOT_FOUND);
	}
}

void SaveEverything(std::string filename)
{
	std::string path = GetWorkingDir() + filename;
	std::string fileContents = "";
	Program* prog = GetProgram();
	for (int i = 0; i < prog->Lines.size(); i++) {
		fileContents.append(prog->Lines[i]->Command);
		fileContents.append("\n");
	}
	File::WriteText(path, fileContents);
}

void LoadEverything(std::string filename)
{
	std::string path = GetWorkingDir() + filename;

	if (File::Exists(path)) {
		auto lines = File::ReadLines(path);
		SetProgramLines(lines);
	}
	else {
		PrintLine(ERR_FILE_NOT_FOUND);
	}
}
