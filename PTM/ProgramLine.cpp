#include "ProgramLine.h"
#include <TBRLGPT.h>
using namespace TBRLGPT;

ProgramLine::ProgramLine()
{
	Command = "";
	LineNumber = 0;
	SourceLineNumber = 0;
	ParamIndex = 0;
}

ProgramLine::~ProgramLine()
{
	for (int i = 0; i < Params.size(); i++) {
		delete Params[i];
		Params[i] = NULL;
	}
	Params.clear();
}

void ProgramLine::AddParam(CommandParam param)
{
	CommandParam* newParam = new CommandParam();
	newParam->Type = param.Type;
	newParam->String = param.String;
	newParam->Number = param.Number;
	Params.push_back(newParam);
}

std::vector<CommandParam*>& ProgramLine::GetParams()
{
	return Params;
}

CommandParam* ProgramLine::GetParam(int index)
{
	return Params[index];
}

CommandParam* ProgramLine::NextParam()
{
	if (ParamIndex >= 0 && ParamIndex < Params.size()) {
		CommandParam* param = Params[ParamIndex];
		ParamIndex++;
		return param;
	}

	return NULL;
}

bool ProgramLine::HasParams()
{
	return !Params.empty();
}

bool ProgramLine::HasParams(int count)
{
	return Params.size() == count;
}
