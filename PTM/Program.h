#pragma once
#include <vector>
#include <string>
#include "ProgramLine.h"

class Program
{
public:
	Program();
	~Program();

	void Parse(std::vector<std::string>& sourceLines);
	std::string Validate();
	int GetSize();
	ProgramLine* GetLine(int lineNumber);

private:
	std::vector<ProgramLine*> Lines;
};
