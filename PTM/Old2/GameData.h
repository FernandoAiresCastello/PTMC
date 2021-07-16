#pragma once
#include <map>
#include <string>
#include <vector>
#include <TBRLGPT.h>
#include "Program.h"
using namespace TBRLGPT;

class GameData
{
public:
	GameData();
	~GameData();

	void SetCharset(Charset* chars);
	void SetPalette(Palette* pal);
	void SetProgram(Program* prog);

private:
	Charset* Chars;
	Palette* Pal;
	std::vector<Scene*> Scenes;
	std::vector<SceneObject*> ObjTemplates;
	Program* Prog;
};
