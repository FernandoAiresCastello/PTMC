#pragma once
#include <map>
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
	bool HasLabel(std::string& label);
	int GetLabel(std::string& label);

private:
	std::vector<ProgramLine*> Lines;
	std::map<std::string, int> Labels;
};
