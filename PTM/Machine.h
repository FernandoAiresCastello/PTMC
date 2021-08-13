#pragma once
#include <Windows.h>
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <SDL.h>
#include <TileGameLib.h>
#include "Machine.h"
#include "ProgramLine.h"
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

	void Abort(std::string msg, bool showSource = true);
	Parameter Pop();
	int PopNumber();
	std::string PopString();
	void InitCommandMap();

	void C_Nop();
	void C_Exit();
	void C_Halt();
	void C_Push();
	void C_WindowOpen();
	void C_WindowClear();
	void C_WindowUpdate();
};
