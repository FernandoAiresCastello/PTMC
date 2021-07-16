#pragma once
#include <string>
#include <vector>
#include <TBRLGPT.h>
using namespace TBRLGPT;

struct Program;
struct ProgramLine;

void InitProgramEditor();
void DestroyProgramEditor();
void DestroyProgram();
void RunProgramEditor();
void AddProgramLine(std::string command);
void FormatProgramLine(ProgramLine* line, bool trim);
void PrintProgramEditor();
struct Program* GetProgram();
void SetProgramLines(std::vector<std::string> lines);
