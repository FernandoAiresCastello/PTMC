#include "Array.h"

Array::Array()
{
}

Array::~Array()
{
}

void Array::Add(StringOrNumber value)
{
	Elements.push_back(value);
}

void Array::Set(int index, StringOrNumber value)
{
	Elements[index] = value;
}

StringOrNumber& Array::Get(int index)
{
	return Elements[index];
}
