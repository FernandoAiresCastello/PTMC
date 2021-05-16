#include "Environment.h"

Environment::Environment()
{
	Util::Randomize();
}

Environment::~Environment()
{
}

void Environment::SetVar(std::string var, std::string value)
{
	Vars[var] = value;
}

void Environment::SetVar(std::string var, int value)
{
	Vars[var] = String::ToString(value);
}

std::string Environment::GetStringVar(std::string var)
{
	return Vars[var];
}

int Environment::GetNumericVar(std::string var)
{
	return String::ToInt(GetStringVar(var));
}

void Environment::PushToCallStack(int programPtr)
{
	CallStack.push(programPtr);
}

int Environment::PopFromCallStack()
{
	int top = CallStack.top();
	CallStack.pop();
	return top;
}
