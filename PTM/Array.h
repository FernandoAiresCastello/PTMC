#pragma once

#include <vector>
#include <string>
#include "StringOrNumber.h"

class Array
{
public:
	Array();
	~Array();

	void Add(StringOrNumber value);
	void Set(int index, StringOrNumber value);
	StringOrNumber& Get(int index);

private:
	std::vector<StringOrNumber> Elements;
};
