#pragma once
#include <string>
#include <TBRLGPT.h>
#include "Program.h"
#include "ProgramLine.h"
#include "Environment.h"

using namespace TBRLGPT;

class Interpreter
{
public:
	Interpreter();
	~Interpreter();

	void Run(class Program* program, Environment* env);

private:
	Environment* Env;
	class Program* Program;
	ProgramLine* CurrentLine;
	int ProgramPtr;
	bool Running;
	bool Branching;
	bool Halted;
	int PausedFor;
	int ParamPtr;

	void Abort(std::string msg);
	void ErrorOutOfParams();
	std::string NextString();
	int NextNumber();
	std::string NextVariableIdentifier();
	void Goto();
	void Call();
	Object* GetSelectedObject();
	void Execute(ProgramLine* line);
};
