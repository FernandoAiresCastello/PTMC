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
		std::string rawParam = src.substr(firstSpaceIx);
		Params.push_back(new Parameter(rawParam));
	}
	else {
		Command = TString::ToUpper(src);
	}
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
