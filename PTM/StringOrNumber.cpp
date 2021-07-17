#include "StringOrNumber.h"
#include <TBRLGPT.h>
using namespace TBRLGPT;

StringOrNumber::StringOrNumber()
{
}

StringOrNumber::StringOrNumber(std::string value)
{
	StringValue = value;
	NumberValue = String::ToInt(value);
}

StringOrNumber::StringOrNumber(int value)
{
	StringValue = String::ToString(value);
	NumberValue = value;
}

StringOrNumber::~StringOrNumber()
{
}
