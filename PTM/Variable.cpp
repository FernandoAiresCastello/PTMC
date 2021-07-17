#include "Variable.h"
#include <TBRLGPT.h>
using namespace TBRLGPT;

Variable::Variable()
{
}

Variable::Variable(std::string name, std::string value)
{
	Name = name;
	StringValue = value;
	NumberValue = String::ToInt(value);
}

Variable::Variable(std::string name, int value)
{
	Name = name;
	StringValue = String::ToString(value);
	NumberValue = value;
}

Variable::~Variable()
{
}
