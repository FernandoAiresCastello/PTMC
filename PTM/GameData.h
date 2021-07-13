#pragma once
#include <map>
#include <string>
#include <vector>
#include <TBRLGPT.h>
using namespace TBRLGPT;

class GameData
{
public:
	GameData(Charset* chars, Palette* pal);
	~GameData();

private:
	Charset* Chars;
	Palette* Pal;
};
