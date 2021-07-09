#include <TBRLGPT.h>
#include "Parameter.h"

using namespace TBRLGPT;

Parameter::Parameter(std::string& value, ParameterType type)
{
	Type = type;
	StringValue = value;
	NumericValue = String::ToInt(value);
}

Parameter::~Parameter()
{
}

ParameterType Parameter::GetType()
{
	return Type;
}

std::string Parameter::GetStringValue()
{
	return StringValue;
}

int Parameter::GetNumericValue()
{
	return NumericValue;
}
