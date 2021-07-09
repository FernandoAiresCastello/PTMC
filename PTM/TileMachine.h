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
void RunProgram(Program* prog);
int GetCommandByteCode(std::string& command);
void ExecuteCompiledProgram();
void ExecuteCompiledProgramLine(CompiledProgramLine& line);
void SetError(std::string error);

void Cmd_Nop(CompiledProgramLine& line);
void Cmd_Push(CompiledProgramLine& line);
void Cmd_Pop(CompiledProgramLine& line);
void Cmd_Mset(CompiledProgramLine& line);
void Cmd_Mget(CompiledProgramLine& line);
void Cmd_Halt(CompiledProgramLine& line);
void Cmd_Exit(CompiledProgramLine& line);
void Cmd_Putc(CompiledProgramLine& line);
void Cmd_Refr(CompiledProgramLine& line);
