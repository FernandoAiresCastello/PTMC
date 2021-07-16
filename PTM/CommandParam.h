#pragma once

#include <string>
#include "CommandParamType.h"

class CommandParam
{
public:
	CommandParamType Type;
	std::string String;
	int Number;

	CommandParam();
	~CommandParam();

	bool IsValid();
};
