#pragma once
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <SDL.h>
#include "Machine.h"
#include "ProgramLine.h"
#include "ObjectTemplate.h"
#include <TileGameLib.h>
using namespace TileGameLib;

class Machine
{
public:
	Machine();
	Machine(const Machine& other) = delete;
	~Machine();

	void LoadProgram(std::string filename);
	void Run();

private:
	bool Running;
	bool Halted;
	std::map<std::string, void(Machine::*)()> CmdMap;
	std::vector<ProgramLine*> Program;
	std::map<std::string, int> Labels;
	std::stack<Parameter> ParamStack;
	int ProgramPtr;
	ProgramLine* CurrentLine;
	TWindow* Win;
	TPalette* Pal;
	TCharset* Chars;
	int BackColor;
	TBoard* Board;
	TBoardView* View;

	struct {
		int X;
		int Y;
		int Layer;
	} BoardCursor;

	std::map<std::string, ObjectTemplate*> ObjTemplates;

	void Abort(std::string msg, bool showSource = true);
	Parameter Pop();
	int PopNumber();
	std::string PopString();
	void InitCommandMap();

	void C_NotImplemented();
	void C_Nop();
	void C_Exit();
	void C_Halt();
	void C_Push();
	void C_DuplicateStackItem();
	void C_WindowOpen();
	void C_WindowClear();
	void C_WindowUpdate();
	void C_PaletteLoad();
	void C_PaletteClear();
	void C_PaletteSet();
	void C_CharsetLoad();
	void C_CharsetClear();
	void C_CharsetSet();
	void C_MapCursorSet();
	void C_MapTileAdd();
	void C_MapObjectTemplatePut();
	void C_ObjectTemplatesLoad();
	void C_ObjectTemplatesLoad2();
	void C_ObjectTemplateCreate();
	void C_ObjectTemplateTileAdd();
};
