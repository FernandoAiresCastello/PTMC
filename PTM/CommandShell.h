#pragma once
#include <string>
#include <vector>

struct ShellCommand;

void InitCommandShell();
void DestroyCommandShell();
void RunCommandShell();
void Debug();
void PrintIntro();
ShellCommand ParseCommandLine(std::string commandLine);
void InterpretCurrentLine();
