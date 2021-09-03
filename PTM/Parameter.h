#pragma once
#include <string>
#include "ParameterType.h"

class Parameter
{
public:
	ParameterType Type = ParameterType::Invalid;
	std::string String = "";
	int Number = 0;

	Parameter();
	Parameter(std::string& str);
	Parameter(std::string& str, ParameterType type);
	Parameter(const Parameter& other);
	~Parameter();

private:
};
