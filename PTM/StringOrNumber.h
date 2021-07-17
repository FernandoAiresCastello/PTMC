#pragma once

#include <string>

class StringOrNumber
{
public:
	StringOrNumber();
	StringOrNumber(std::string value);
	StringOrNumber(int value);
	~StringOrNumber();

	std::string StringValue;
	int NumberValue;
};
