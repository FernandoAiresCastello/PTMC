#pragma once

#include <string>
#include <vector>
#include "CommandParam.h"

class ProgramLine
{
public:
	std::string Command;
	std::vector<CommandParam*> Params;
	int LineNumber;
	int SourceLineNumber;

	ProgramLine();
	~ProgramLine();

	void AddParam(CommandParam param);
	std::vector<CommandParam*>& GetParams();
	CommandParam* NextParam();
	bool HasParams();
	bool HasParams(int count);

private:
	int ParamIndex;
};
