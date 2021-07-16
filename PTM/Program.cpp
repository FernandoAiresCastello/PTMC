#include "Program.h"
#include <TBRLGPT.h>
using namespace TBRLGPT;

Program::Program()
{
}

Program::~Program()
{
	Clear();
}

void Program::Clear()
{
	for (int i = 0; i < Lines.size(); i++) {
		delete Lines[i];
		Lines[i] = NULL;
	}
	Lines.clear();
}

ProgramLine* Program::GetLine(int line)
{
	return Lines[line];
}

int Program::GetLabelTarget(std::string label)
{
	return Labels[label];
}

void Program::Load(std::string file)
{
	Parse(File::ReadText(file));
}

void Program::Parse(std::string src)
{
	Clear();
	
	int sourceLineNumber = 0;
	int actualLineNumber = 0;
	auto lines = String::Split(src, '\n');
	
	for (auto& srcLine : lines) {
		std::string line = String::Trim(srcLine);
		sourceLineNumber++;

		if (line.empty()) {
			continue;
		}
		if (String::StartsWith(line, "//")) {
			continue;
		}
		if (String::EndsWith(line, ":")) {
			Labels[String::RemoveLast(line)] = actualLineNumber;
			continue;
		}

		Lines.push_back(ParseLine(line, sourceLineNumber, actualLineNumber));
		actualLineNumber++;
	}
}

ProgramLine* Program::ParseLine(std::string src, int sourceLineNumber, int actualLineNumber)
{
	ProgramLine* line = new ProgramLine();
	line->LineNumber = actualLineNumber;
	line->SourceLineNumber = sourceLineNumber;

	int ixFirstSpace = src.find(' ');
	line->Command = String::ToUpper(String::Trim(src.substr(0, ixFirstSpace)));
	
	if (ixFirstSpace < 0)
		return line;

	std::string rawParams = String::Trim(src.substr(ixFirstSpace));

	if (String::Contains(rawParams, "\"")) {
		CommandParam param;
		param.Type = CommandParamType::StringLiteral;
		param.String = String::RemoveFirstAndLast(rawParams);
		param.Number = 0;
		line->AddParam(param);
	}
	else {
		auto rawParamList = String::Split(rawParams, ',');
		for (auto& rawParam : rawParamList) {
			CommandParam param;
			param.String = String::Trim(rawParam);

			if (String::IsNumber(param.String)) {
				param.Type = CommandParamType::NumberLiteral;
				param.Number = String::ToInt(param.String);
			}
			else if (String::StartsWith(param.String, "0x")) {
				param.Type = CommandParamType::NumberLiteral;
				param.Number = String::ToInt(param.String);
				param.String = String::ToString(param.Number);
			}
			else if (String::StartsWith(param.String, "0b")) {
				param.Type = CommandParamType::NumberLiteral;
				param.Number = String::BinaryToInt(param.String);
				param.String = String::ToString(param.Number);
			}
			else if (String::StartsWith(param.String, "$")) {
				param.Type = CommandParamType::Variable;
				param.String = String::Skip(param.String, 1);
			}
			else {
				param.Type = CommandParamType::Label;
			}

			line->AddParam(param);
		}
	}

	return line;
}
