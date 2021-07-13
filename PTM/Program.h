#pragma once
#include <string>
#include <vector>
#include <TBRLGPT.h>
using namespace TBRLGPT;

struct ProgramLine {
	std::string Command;
};

struct Program {
	std::vector<ProgramLine*> Lines;
};

struct CompiledProgramLine {
	int SourceLineNumber;
	int ActualLineIndex;
	std::string Command;
	bool HasParam;
	std::string ParamString;
	int ParamNumber;
};
