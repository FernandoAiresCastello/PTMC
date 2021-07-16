#include "CommandParam.h"

CommandParam::CommandParam()
{
	Type = CommandParamType::Invalid;
	String = "";
	Number = 0;
}

CommandParam::~CommandParam()
{
}

bool CommandParam::IsValid()
{
	return Type != CommandParamType::Invalid;
}
