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
	Charset* GetCharset();
	Palette* GetPalette();
	std::vector<SceneObject*>& GetTemplates();
	void SetTemplate(std::string id, SceneObject* o);
	SceneObject* GetTemplate(std::string id);
	void DeleteObjTemplate(std::string id);

private:
	Charset* Chars;
	Palette* Pal;
	std::vector<Scene*> Scenes;
	std::vector<SceneObject*> ObjTemplates;
	Program* Prog;
};
