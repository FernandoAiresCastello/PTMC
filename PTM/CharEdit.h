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

	CharEdit(EditorBase* baseEditor, Graphics* gr, Datafile* data, int ch);
	~CharEdit();

	void Draw();
	void HandleKeyEvents();

private:
	EditorBase* BaseEditor;
	Graphics* Gr;
	Datafile* Data;
	int CharIndex;
	char Buffer[64];
	int CursorX;
	int CursorY;

	void ToggleCurrentPixel();
	void SetCurrentPixel(int value);
	void ClearChar();
	void UpdateBuffer();
	void UpdateChar();
};
