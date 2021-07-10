#pragma once
#include <string>
#include <vector>
#include <TBRLGPT.h>
using namespace TBRLGPT;

struct ShellCommand;

void InitCommandShell();
void DestroyCommandShell();
void RunCommandShell();
void Debug();
void PrintIntro();
ShellCommand ParseCommandLine(std::string commandLine);
void InterpretCurrentLine();
void PrintFiles(std::string pattern);
