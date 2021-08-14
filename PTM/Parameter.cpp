#include "Parameter.h"
#include <TileGameLib.h>
using namespace TileGameLib;

Parameter::Parameter()
{
}

Parameter::Parameter(std::string& str)
{
	str = TString::Trim(str);

	if (TString::IsNumber(str) || TString::StartsWith(str, "0x") || TString::StartsWith(str, "0b")) {
		Type = ParameterType::Number;
		Number = TString::ToInt(str);
		String = TString::ToString(Number);
	}
	else if (TString::StartsAndEndsWith(str, '"')) {
		Type = ParameterType::String;
		Number = 0;
		String = TString::RemoveFirstAndLast(str);
	}
	else if (TString::StartsWith(str, '$')) {
		Type = ParameterType::Variable;
		Number = 0;
		String = TString::Skip(str, 1);
	}
	else {
		Type = ParameterType::Identifier;
		Number = 0;
		String = str;
	}
}

Parameter::Parameter(const Parameter& other)
{
	Type = other.Type;
	String = other.String;
	Number = other.Number;
}

Parameter::~Parameter()
{
}
