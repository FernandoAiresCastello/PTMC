#pragma once
#include <string>
#include <vector>
#include <TBRLGPT.h>
using namespace TBRLGPT;

struct ProgramLine;

void InitProgramEditor();
void DestroyProgramEditor();
void RunProgramEditor();
void AddProgramLine(std::string command);
void FormatProgramLine(ProgramLine* line, bool trim);
void PrintProgramEditor();
