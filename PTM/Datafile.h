#pragma once

#include <string>
#include <vector>
#include <TBRLGPT.h>
#include "Program.h"
using namespace TBRLGPT;

class Datafile
{
public:
	Datafile();
	~Datafile();

	Program* GetProgram();
	void SetProgram(Program* prog);

private:
	Charset* Chars;
	Palette* Pal;
	std::vector<Scene*> Scenes;
	std::vector<SceneObject*> ObjTemplates;
	Program* Prog;
};
