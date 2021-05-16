#pragma once
#include <string>
#include "Program.h"
#include "ProgramLine.h"
#include "Environment.h"

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
	bool Paused;
	int ParamPtr;

	void FatalError(std::string msg);
	std::string NextStringParam();
	int NextNumericParam();
	void Execute(ProgramLine* line);
};
