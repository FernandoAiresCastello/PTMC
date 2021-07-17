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
	std::map<std::string, Variable> Vars;
	Graphics* Gr;

	void ProcessEvents();
	void OnKeyPress(SDL_Keycode key);
	void Abort(std::string error);
	void Halt();
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
};
