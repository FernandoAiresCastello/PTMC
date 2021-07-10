#pragma once
#include <string>
#include <vector>
#include <TBRLGPT.h>
#include "Program.h"
using namespace TBRLGPT;

void InitTileMachine();
void DestroyTileMachine();
void RestartTileMachine();
std::string GetTileMachineError();
void RefreshScreen();
void CompileAndRunProgram(Program* prog);
int GetCommandByteCode(std::string& command);
void ExecuteCompiledProgram();
void ExecuteCompiledProgramLine(CompiledProgramLine& line);
void SetError(std::string error);
void BranchTo(std::string label);

void C_Nop();
void C_Push();
void C_Pop();
void C_Mset();
void C_Mget();
void C_Brk();
void C_Halt();
void C_Exit();
void C_Putc();
void C_Refr();
void C_Add();
void C_Sub();
void C_Jp();
void C_Wait();
