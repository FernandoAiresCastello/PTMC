#pragma once
#include <string>
#include "Program.h"

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
int GetCommandByteCode(std::string& command);
void ExecuteCompiledProgram();
void ExecuteCompiledProgramLine(CompiledProgramLine& line);

// Command implementation

void _0x00();
void _0x01();
void _0x02();

void _0x07();
void _0x08();
void _0x09();
void _0x0a();
void _0x0b();

void _0xfd();
void _0xfe();
void _0xff();
