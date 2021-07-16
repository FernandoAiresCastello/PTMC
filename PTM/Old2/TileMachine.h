#pragma once
#include <map>
#include <stack>
#include <vector>
#include <string>
#include "Program.h"
#include "GameData.h"
#include <TBRLGPT.h>
using namespace TBRLGPT;

struct Machine {
	std::string Error = "";
	std::vector<CompiledProgramLine> Program;
	int ProgramPtr = 0;
	CompiledProgramLine* Line = NULL;
	std::map<std::string, std::string> Defs;
	std::map<std::string, int> ProgramLabels;
	bool Branching = false;
	bool Running = false;
	bool Halted = false;
	std::stack<int> Stack;
	Graphics* Gr;
	int BackColor = 0;
	GameData* GameData;
};

extern struct Machine* Machine;

void InitTileMachine();
void DestroyTileMachine();
void RestartTileMachine();
std::string GetTileMachineError();
void ClearMachineScreen();
void RefreshMachineScreen();
void SetError(std::string error);
void BranchTo(std::string label);
bool HasLabel(std::string label);
bool HasDef(std::string def);
void CompileAndRunProgram(Program* prog);
void ExecuteCompiledProgram();

#define ERR_PROGRAM_EMPTY "Program empty"
#define ERR_SYNTAX_ERROR "Syntax error"
#define ERR_INVALID_COMMAND "Invalid command"
#define ERR_END_OF_PROGRAM "End of program"
#define ERR_ILLEGAL_ADDRESS "Illegal address"
#define ERR_STACK_EMPTY "Stack empty"
#define ERR_LABEL_NOT_FOUND "Label not found"
#define ERR_DEF_NOT_FOUND "Def not found"

#define POP Machine->Stack.top(); Machine->Stack.pop()
#define PUSH(x) Machine->Stack.push(x)
