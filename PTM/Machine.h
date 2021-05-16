#pragma once
#include <string>
#include <TBRLGPT.h>
#include "Util.h"
#include "ProgramLoader.h"
#include "Interpreter.h"

using namespace TBRLGPT;

class Machine
{
public:
	Machine();
	~Machine();

	void Run(std::string programFile);

private:
};
