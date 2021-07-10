#pragma once
#include <string>
#include "Program.h"

void InitTileMachine();
void DestroyTileMachine();
void RestartTileMachine();
std::string GetTileMachineError();
void ClearMachineScreen();
void RefreshMachineScreen();
void CompileAndRunProgram(Program* prog);
int GetCommandByteCode(std::string& command);
void ExecuteCompiledProgram();
void ExecuteCompiledProgramLine(CompiledProgramLine& line);
void SetError(std::string error);
void BranchTo(std::string label);
bool HasLabel(std::string label);
bool HasDef(std::string def);

void C_Nop();
void C_Push();
void C_Pop();
void C_Brk();
void C_Halt();
void C_Exit();
void C_Refr();
void C_Add();
void C_Sub();
void C_Jp();
void C_Wait();
