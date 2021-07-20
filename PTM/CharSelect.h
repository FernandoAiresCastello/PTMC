#pragma once

#include <TBRLGPT.h>
#include "Datafile.h"
using namespace TBRLGPT;

class CharSelect
{
public:
	bool Running;
	int X;
	int Y;
	int Width;
	int Height;
	int ForeColor;
	int BackColor;
	bool Cancelled;

	CharSelect(Graphics* gr, Datafile* data);
	~CharSelect();

	void Draw();
	void HandleEvents();
	void InitChars();
	int GetSelectedChar();

private:
	Graphics* Gr;
	Datafile* Data;
	Scene* CharScene;
	SceneView* View;
	SceneObject* Cursor;

	void UpdateCursor();
};
