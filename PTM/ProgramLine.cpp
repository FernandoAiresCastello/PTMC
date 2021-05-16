#include <TBRLGPT.h>
#include "ProgramLine.h"

using namespace TBRLGPT;

ProgramLine::ProgramLine(int lineNumber, int sourceLineNumber, std::string& sourceLine)
	: LineNumber(lineNumber), SourceLineNumber(sourceLineNumber), SourceLine(sourceLine)
{
	Parse(sourceLine);
}

ProgramLine::~ProgramLine()
{
	for (int i = 0; i < Params.size(); i++) {
		delete Params.at(i);
	}
}

bool ProgramLine::ValidateParams()
{
	for (int i = 0; i < Params.size(); i++) {
		Parameter* param = Params.at(i);
		if (param->GetType() == ParameterType::PARAM_INVALID)
			return false;
	}

	return true;
}

int ProgramLine::GetSourceLineNumber()
{
	return SourceLineNumber;
}

int ProgramLine::GetLineNumber()
{
	return LineNumber;
}

std::string& ProgramLine::GetSourceLine()
{
	return SourceLine;
}

std::string& ProgramLine::GetCommand()
{
	return Command;
}

Parameter* ProgramLine::GetParam(int index)
{
	return Params.at(index);
}

int ProgramLine::GetParamCount()
{
	return ParamCount;
}

void ProgramLine::Parse(std::string& sourceLine)
{
	sourceLine = String::Trim(sourceLine);
	sourceLine = String::Replace(sourceLine, "\t", " ");
	int ixFirstSpace = String::FindFirst(sourceLine, ' ');

	Command = String::ToUpper(String::Trim(
		ixFirstSpace >= 0 ? sourceLine.substr(0, ixFirstSpace) : sourceLine));
	
	// If there are no parameters then just return
	if (ixFirstSpace < 0) {
		ParamCount = 0;
		return;
	}

	std::string paramSequence = String::Trim(sourceLine.substr(ixFirstSpace));
	std::vector<std::string> rawParams;

	// Are there any strings literals?
	if (String::Contains(paramSequence, '\"')) {
		std::string curRawParam;
		bool openQuote = false;
		for (int i = 0; i < paramSequence.size(); i++) {
			char ch = paramSequence[i];
			if (ch == '\"') {
				curRawParam.push_back(ch);
				if (openQuote) {
					openQuote = false;
					continue;
				}
				else {
					openQuote = true;
					continue;
				}
			}
			else if (ch == ',' && !openQuote) {
				rawParams.push_back(String::Trim(curRawParam));
				curRawParam = "";
				continue;
			}
			else {
				curRawParam.push_back(ch);
				continue;
			}
		}

		rawParams.push_back(String::Trim(curRawParam));
	}
	// There are no string literals
	else {
		rawParams = String::Split(paramSequence, ',');
	}
	
	// Add parsed parameters
	for (int i = 0; i < rawParams.size(); i++) {
		std::string rawParam = rawParams[i];
		ParameterType type = ParameterType::PARAM_INVALID;

		if (String::StartsWith(rawParam, "0b")) {
			type = ParameterType::PARAM_NUMERIC;
			rawParam = String::ToString(String::BinaryToInt(rawParam));
		}
		else if (String::StartsWith(rawParam, "0x") || String::StartsWithNumber(rawParam)) {
			type = ParameterType::PARAM_NUMERIC;
			rawParam = String::ToString(String::ToInt(rawParam));
		}
		else if (String::StartsWith(rawParam, "\"") && String::EndsWith(rawParam, "\"")) {
			type = ParameterType::PARAM_STRINGLITERAL;
			rawParam = String::RemoveFirstAndLast(rawParam);
		}
		else if (String::StartsWith(rawParam, "$")) {
			type = ParameterType::PARAM_VARIABLE;
			rawParam = String::RemoveFirst(rawParam);
		}

		Params.push_back(new Parameter(rawParam, type));
	}
	
	ParamCount = Params.size();
}
