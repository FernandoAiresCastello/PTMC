#pragma once
#include <string>
#include <vector>
#include "Parameter.h"

class ProgramLine
{
public:
	ProgramLine(int lineNumber, int sourceLineNumber, std::string& sourceLine);
	~ProgramLine();

	bool ValidateParams();
	int GetSourceLineNumber();

private:
	int LineNumber;
	int SourceLineNumber;
	std::string SourceLine;
	std::string Command;
	std::vector<Parameter> Params;
	int ParamCount;

	void Parse(std::string& sourceLine);
};
