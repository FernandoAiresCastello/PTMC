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

	void Clear();
	ProgramLine* GetLine(int line);
	int GetLabelTarget(std::string label);
	void Load(std::string file);
	void Parse(std::string src);

private:
	std::vector<ProgramLine*> Lines;
	std::map<std::string, int> Labels;

	ProgramLine* ParseLine(std::string src, int sourceLineNumber, int actualLineNumber);
};
