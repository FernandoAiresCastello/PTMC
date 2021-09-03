#pragma once

#include <string>

class Variable
{
public:
	std::string Name = "";
	std::string String = "";
	int Number = 0;

	Variable();
	Variable(std::string name, std::string value);
	Variable(const Variable& other);
	~Variable();

private:
};
