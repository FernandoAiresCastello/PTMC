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

int Program::GetSize()
{
	return Lines.size();
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

	std::string literalString = "";
	if (String::Contains(rawParams, "\"")) {
		// debug
		/*if (String::Contains(rawParams, "\"\"")) {
			__debugbreak();
		}*/
		// enddebug
		int ixOpenQuote = rawParams.find('\"');
		int ixCloseQuote = rawParams.find('\"', ixOpenQuote + 1);
		std::string temp = rawParams.substr(ixOpenQuote, ixCloseQuote);
		literalString = String::Replace(temp, "\"", "");
		if (!literalString.empty())
			rawParams = String::Replace(rawParams, literalString, "");
	}

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
		else if (param.String == "\"\"") {
			param.Type = CommandParamType::StringLiteral;
			param.String = literalString;
		}
		else {
			param.Type = CommandParamType::Label;
		}

		line->AddParam(param);
	}
	
	return line;
}
