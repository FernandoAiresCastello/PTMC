#pragma once
#include <map>
#include <string>
#include <TBRLGPT.h>

using namespace TBRLGPT;

class Environment
{
public:
	Graphics* Gr;
	UIContext* Ui;
	int GfxCursorX;
	int GfxCursorY;

	Environment();
	~Environment();

	void SetVar(std::string var, std::string value);
	void SetVar(std::string var, int value);
	std::string GetStringVar(std::string var);
	int GetNumericVar(std::string var);

private:
	Project* Proj;
	std::map<std::string, std::string> Vars;
};
