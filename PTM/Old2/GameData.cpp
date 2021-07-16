#include "GameData.h"

GameData::GameData()
{
}

GameData::~GameData()
{
}

void GameData::SetCharset(Charset* chars)
{
	Chars = chars;
}

void GameData::SetPalette(Palette* pal)
{
	Pal = pal;
}

void GameData::SetProgram(Program* prog)
{
	Prog = prog;
}
