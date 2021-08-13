#pragma once
#include <vector>
#include <string>
#include "Parameter.h"

class ProgramLine
{
public:
	int SourceLineNumber;
	int ActualLineNumber;
	std::string Source;
	std::string Command;

	ProgramLine();
	ProgramLine(int srcLine, int actualLine, std::string& src);
	~ProgramLine();

	void Parse(std::string& src);
	Parameter* NextParam();
	Parameter* GetParam(int ix = 0);

private:
	ProgramLine(const ProgramLine& other) = delete;

	std::vector<Parameter*> Params;
	int ParamIndex = 0;
};
