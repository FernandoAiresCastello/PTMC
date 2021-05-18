#include "Environment.h"

Environment::Environment()
{
	Util::Randomize();
}

Environment::~Environment()
{
	for (auto it = Maps.begin(); it != Maps.end(); ++it) {
		delete it->second;
	}
	Maps.clear();
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

void Environment::AddMap(std::string& id, Map* map)
{
	Maps[id] = map;
}

Map* Environment::GetMap(std::string& id)
{
	return Maps[id];
}

void Environment::AddView(std::string& id, MapViewport* view)
{
	Views[id] = view;
}

MapViewport* Environment::GetView(std::string & id)
{
	return Views[id];
}
