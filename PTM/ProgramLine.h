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
	std::string SourceLine;

	ProgramLine();
	~ProgramLine();

	void AddParam(CommandParam param);
	std::vector<CommandParam*>& GetParams();
	CommandParam* GetParam(int index);
	CommandParam* NextParam();
	bool HasParams();
	bool HasParams(int count);
	void ResetParamIndex();

private:
	int ParamIndex;
};
