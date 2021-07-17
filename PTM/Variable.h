#pragma once

#include <string>

class Variable
{
public:
	Variable();
	Variable(std::string name, std::string value);
	Variable(std::string name, int value);
	~Variable();

	std::string Name;
	std::string StringValue;
	int NumberValue;
};
