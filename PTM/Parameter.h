#pragma once
#include <string>

enum ParameterType
{
	PARAM_INVALID,
	PARAM_NUMERIC,
	PARAM_STRINGLITERAL,
	PARAM_VARIABLE
};

class Parameter
{
public:
	Parameter(std::string& value, ParameterType type);
	~Parameter();

	ParameterType GetType();

private:
	ParameterType Type;
	std::string StringValue;
	int NumericValue;
};
