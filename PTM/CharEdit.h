#pragma once

#include <TBRLGPT.h>
#include "Datafile.h"
#include "EditorBase.h"
using namespace TBRLGPT;

class CharEdit
{
public:
	bool Running;
	static int X;
	static int Y;

	CharEdit(Graphics* gr, Datafile* data, int ch, int fgc, int bgc);
	~CharEdit();

	void Draw();
	void HandleEvents();

private:
	Graphics* Gr;
	Datafile* Data;
	int CharIndex;
	int ForeColor;
	int BackColor;
	char Buffer[64];
	char OriginalPixels[64];
	int CursorX;
	int CursorY;

	void ToggleCurrentPixel();
	void SetCurrentPixel(int value);
	void ClearChar();
	void InitPixelBuffer();
	void UpdateChar();
	void RevertChar();
};
