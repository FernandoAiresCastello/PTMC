#pragma once

#include <TBRLGPT.h>
#include "Datafile.h"
using namespace TBRLGPT;

class Debugger
{
public:
	Debugger(Datafile* datafile, Graphics* gr);
	~Debugger();

	void Run();

private:
	Datafile* DataFile;
	Graphics* Gr;
	bool Running;
};
