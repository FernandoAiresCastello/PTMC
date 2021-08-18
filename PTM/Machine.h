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

#define CMD(cmd,fn) CmdMap[cmd] = &Machine::fn

class Machine
{
public:
	Machine();
	~Machine();

	void LoadProgram(std::string filename);
	void Run();

private:
	Machine(const Machine& other) = delete;

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
	std::map<std::string, TBoard*> Maps;
	std::map<std::string, TBoardView*> Views;
	TBoard* SelectedMap;

	struct {
		int X;
		int Y;
		int Layer;
	} MapCursor;

	std::map<std::string, ObjectTemplate*> ObjTemplates;

	void Abort(std::string msg, bool showSource = true);
	Parameter Pop();
	int PopNumber();
	std::string PopString();
	void InitCommandMap();

	void C_NotImplemented();
	void C_Nop();
	void C_Breakpoint();
	void C_Exit();
	void C_Halt();
	void C_Push();
	void C_DuplicateStackItem();
	void C_Pop();
	void C_WindowOpen();
	void C_WindowClear();
	void C_WindowUpdate();
	void C_PaletteLoad();
	void C_PaletteClear();
	void C_PaletteSet();
	void C_CharsetLoad();
	void C_CharsetClear();
	void C_CharsetSet();
	void C_ObjectTemplatesLoad();
	void C_ObjectTemplateCreate();
	void C_ObjectTemplateTileAdd();
	void C_MapCreate();
	void C_MapSelect();
	void C_MapViewCreate();
	void C_MapViewEnable();
	void C_MapViewDisable();
	void C_MapLoad();
	void C_MapCursorSet();
	void C_MapTileAdd();
	void C_MapObjectTemplatePut();
	void C_MapPutObjectString();
};
