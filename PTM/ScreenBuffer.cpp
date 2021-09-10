#include "ScreenBuffer.h"

ScreenBuffer::ScreenBuffer(int cols, int rows)
{
	Size = cols * rows;
	Cols = cols;
	Rows = rows;
	Cells = new ScreenCell[Size];
}

ScreenBuffer::~ScreenBuffer()
{
	delete[] Cells;
}
