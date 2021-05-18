#include <TBRLGPT.h>
#include "Program.h"

using namespace TBRLGPT;

Program::Program()
{
}

Program::~Program()
{
	for (int i = 0; i < Lines.size(); i++) {
		delete Lines.at(i);
	}
}

void Program::Parse(std::vector<std::string>& sourceLines)
{
	int actualLineNum = 0;
	for (int srcLineNum = 0; srcLineNum < sourceLines.size(); srcLineNum++) {
		std::string srcLine = String::Trim(sourceLines[srcLineNum]);
		if (!srcLine.empty() && !String::StartsWith(srcLine, "//")) {
			if (String::EndsWith(srcLine, ":")) {
				std::string label = String::RemoveLast(srcLine);
				Labels[label] = actualLineNum;
			}
			else {
				ProgramLine* line = new ProgramLine(actualLineNum, srcLineNum + 1, srcLine);
				Lines.push_back(line);
				actualLineNum++;
			}
		}
	}
}

std::string Program::Validate()
{
	std::string validation = "";

	for (int i = 0; i < Lines.size(); i++) {
		ProgramLine* line = Lines.at(i);
		if (!line->ValidateParams()) {
			validation = String::Format("Syntax error in line %i:\n\n%s", 
				line->GetSourceLineNumber(), line->GetSourceLine().c_str());
			break;
		}
	}

	return validation;
}

int Program::GetSize()
{
	return Lines.size();
}

ProgramLine* Program::GetLine(int lineNumber)
{
	return Lines.at(lineNumber);
}

int Program::GetLabel(std::string label)
{
	return Labels[label];
}
