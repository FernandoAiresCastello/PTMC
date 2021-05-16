#pragma once
#include <map>
#include <stack>
#include <string>
#include <TBRLGPT.h>

using namespace TBRLGPT;

class Environment
{
public:
	int Cycles = 0;
	Graphics* Gr = NULL;
	UIContext* Ui = NULL;
	int GfxCursorX = 0;
	int GfxCursorY = 0;

	Environment();
	~Environment();

	void SetVar(std::string var, std::string value);
	void SetVar(std::string var, int value);
	std::string GetStringVar(std::string var);
	int GetNumericVar(std::string var);
	void PushToCallStack(int programPtr);
	int PopFromCallStack();

private:
	Project* Proj;
	std::map<std::string, std::string> Vars;
	std::stack<int> CallStack;
};
