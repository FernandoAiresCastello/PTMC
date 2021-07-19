#pragma once

#include <TBRLGPT.h>
#include "Datafile.h"
using namespace TBRLGPT;

class CharEdit
{
public:
	bool Running;
	int X;
	int Y;
	int CharIndex;
	int ForeColor;
	int BackColor;

	CharEdit(Graphics* gr, Datafile* data);
	~CharEdit();

	void Draw();
	void HandleEvents();
	void InitPixelBuffer();

private:
	Graphics* Gr;
	Datafile* Data;
	char Buffer[64];
	char OriginalPixels[64];
	int CursorX;
	int CursorY;

	void ToggleCurrentPixel();
	void SetCurrentPixel(int value);
	void ClearChar();
	void UpdateChar();
	void RevertChar();
};
