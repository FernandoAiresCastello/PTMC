#include <TBRLGPT.h>
#include "Program.h"

using namespace TBRLGPT;

Program::Program()
{
}

Program::~Program()
{
}

void Program::Parse(std::vector<std::string>& sourceLines)
{
	int actualLineNum = 0;
	for (int srcLineNum = 0; srcLineNum < sourceLines.size(); srcLineNum++) {
		std::string srcLine = String::Trim(sourceLines[srcLineNum]);
		if (!srcLine.empty() && !String::StartsWith(srcLine, "//")) {
			ProgramLine line(actualLineNum, srcLineNum + 1, srcLine);
			Lines.push_back(line);
			actualLineNum++;
		}
	}
}

std::string Program::Validate()
{
	std::string validation = "";

	for (int i = 0; i < Lines.size(); i++) {
		ProgramLine& line = Lines[i];
		if (!line.ValidateParams()) {
			validation = String::Format("Syntax error in line %i", line.GetSourceLineNumber());
			break;
		}
	}

	return validation;
}
