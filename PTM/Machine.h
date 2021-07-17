#pragma once

#include <map>
#include <string>
#include <vector>
#include <stack>
#include "Program.h"
#include "ProgramLine.h"
#include "Datafile.h"
#include <TBRLGPT.h>
using namespace TBRLGPT;

class Machine
{
public:
	Machine();
	~Machine();

	void Run(Datafile* data);

private:
	bool Running;
	int ProgramPtr;
	std::map<std::string, void (Machine::*)(void)> Cmd;
	ProgramLine* Line;
	bool Branch;
	std::string Error;
	std::stack<CommandParam*> Stack;

	void InitCommandMap();
	void SetError(std::string error);
	void Push(CommandParam* param);
	CommandParam* Pop();
	
	void C_Nop();
	void C_Push();
};
