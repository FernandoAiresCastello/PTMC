#pragma once

#include <map>
#include <string>
#include <vector>
#include <stack>
#include <TBRLGPT.h>
#include "Program.h"
#include "ProgramLine.h"
#include "Datafile.h"
#include "Variable.h"
#include "StackElement.h"
#include "Debugger.h"
using namespace TBRLGPT;

class Machine
{
public:
	Machine();
	~Machine();

	void Run(Datafile* data, Graphics* gr);

private:
	bool Running;
	Program* Prog;
	int ProgramPtr;
	std::map<std::string, void (Machine::*)(void)> Cmd;
	ProgramLine* Line;
	bool Branch;
	std::string Error;
	std::stack<StackElement> Stack;
	std::stack<int> CallStack;
	std::map<std::string, Variable> Vars;
	Graphics* Gr;
	Charset* DefaultChars;
	Debugger* Debugger;

	void ProcessEvents();
	void OnKeyPress(SDL_Keycode key);
	void Abort(std::string error);
	void Halt();
	void Goto(std::string label);
	void Call(std::string label);
	void Return();
	void Print(std::string text, int x, int y, int fgc, int bgc);
	void ClearScreen(int bgc);
	void UpdateScreen();
	void PushString(std::string value);
	void PushNumber(int value);
	std::string PopString();
	int PopNumber();
	void SetVar(std::string name, std::string value);
	void SetVar(std::string name, int value);
	std::string GetVarAsString(std::string name);
	int GetVarAsNumber(std::string name);
	void InitCommandMap();

	void C_Nop();
	void C_Brk();
	void C_Halt();
	void C_Push();
	void C_Exit();
	void C_Var();
	void C_Debug();
	void C_Call();
	void C_Goto();
	void C_Return();
};
