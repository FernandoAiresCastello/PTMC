#include "ProgramLine.h"
#include <sstream>
#include <TileGameLib.h>
using namespace TileGameLib;

ProgramLine::ProgramLine()
{
}

ProgramLine::ProgramLine(int srcLine, int actualLine, std::string& src)
{
	SourceLineNumber = srcLine;
	ActualLineNumber = actualLine;
	Source = TString::Trim(src);
	Parse(src);
}

ProgramLine::~ProgramLine()
{
	for (int i = 0; i < Params.size(); i++) {
		delete Params[i];
		Params[i] = nullptr;
	}
	Params.clear();
}

void ProgramLine::Parse(std::string& src)
{
	Params.clear();
	src = TString::Trim(src);
	int firstSpaceIx = TString::FindFirst(src, ' ');

	if (firstSpaceIx >= 0) {
		Command = TString::ToUpper(src.substr(0, firstSpaceIx));
		std::string rawParams = src.substr(firstSpaceIx);
		ParseParams(rawParams);
	}
	else {
		Command = TString::ToUpper(src);
	}
}

void ProgramLine::ParseParams(std::string& rawParams)
{
	if (!TString::Contains(rawParams, '"')) {
		auto params = TString::Split(rawParams, ' ');
		for (auto& param : params) {
			if (!param.empty())
				Params.push_back(new Parameter(param));
		}
		return;
	}

	std::stringstream curParam;
	bool quote = false;
	char ch = 0;
	int i = 0;

	while (true) {
		ch = rawParams[i];
		i++;
		if (ch == '"') {
			quote = !quote;
			curParam.put(ch);
		}
		else if (ch == ' ' && !quote) {
			std::string param = curParam.str();
			curParam.str("");
			if (!param.empty())
				Params.push_back(new Parameter(param));
		}
		else {
			curParam.put(ch);
		}
		if (i >= rawParams.size()) {
			break;
		}
	}
}

int ProgramLine::GetParamCount()
{
	return Params.size();
}

Parameter* ProgramLine::NextParam()
{
	Parameter* param = Params[ParamIndex];
	
	ParamIndex++;
	if (ParamIndex >= Params.size())
		ParamIndex = 0;

	return param;
}

Parameter* ProgramLine::GetParam(int ix)
{
	if (ix >= 0 && ix < Params.size())
		return Params[ix];

	return nullptr;
}
