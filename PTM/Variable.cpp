#include "Variable.h"
#include <TileGameLib.h>
using namespace TileGameLib;

Variable::Variable()
{
}

Variable::Variable(std::string name, std::string value)
{
	Name = name;
	String = value;
	Number = TString::ToInt(value);
}

Variable::Variable(const Variable& other)
{
	Name = other.Name;
	Number = other.Number;
	String = other.String;
}

Variable::~Variable()
{
}
