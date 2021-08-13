#pragma once
#include <map>
#include <TileGameLib.h>
using namespace TileGameLib;

class ProgramEditor
{
public:
	ProgramEditor();
	~ProgramEditor();

	void Run();

private:
	bool Running;
	std::map<SDL_Keycode, void(ProgramEditor::*)()> Keymap;
	TWindow* Wnd;

	void InitKeyMapping();
	void Draw();
	void Quit();
};
