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
	int GetLineNumber();
	std::string& GetSourceLine();
	std::string& GetCommand();
	Parameter* GetParam(int index);
	int GetParamCount();

private:
	int LineNumber;
	int SourceLineNumber;
	std::string SourceLine;
	std::string Command;
	std::vector<Parameter*> Params;
	int ParamCount;

	void Parse(std::string& sourceLine);
};
