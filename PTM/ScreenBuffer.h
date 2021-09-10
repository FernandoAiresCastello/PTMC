#pragma once
#include <vector>
#include <string>
#include <TileGameLib.h>
using namespace TileGameLib;

struct ScreenCell {
	int Char;
	int ForeColor;
	int BackColor;
};

class ScreenBuffer
{
public:
	ScreenBuffer(int cols, int rows);
	~ScreenBuffer();

	int Size = 0;
	int Cols = 0;
	int Rows = 0;
	ScreenCell* Cells = nullptr;
};
