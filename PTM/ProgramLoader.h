#pragma once
#include <string>
#include "Program.h"

class ProgramLoader
{
public:
	ProgramLoader();
	~ProgramLoader();

	Program* Load(std::string filename);
};
