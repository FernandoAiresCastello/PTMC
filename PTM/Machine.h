#pragma once
#include "Program.h"
#include <map>
#include <vector>
#include <string>
#include <stack>
#include <TileGameLib.h>
using namespace TileGameLib;

extern std::map<std::string, byte> CmdMap;
#define OP(cmd,opc,fn) Opcodes[opc] = &Machine::fn; CmdMap[cmd] = opc;

class Machine
{
private:
	// Opcodes
	void O_Nop();
	void O_Push();
	void O_WindowCreate();
	void O_Break();
	void O_Halt();
	void O_Exit();

public:
	Machine();
	~Machine();

	void Run(Program* prog);

private:
	const int MemorySize = 0x10000;
	const byte NullOpcode = 0;
	std::map<int, void(Machine::*)()> Opcodes;
	bool Running = false;
	Program* Prog;
	int ProgramPtr = 0;
	int* Memory;
	std::stack<int> ParamStack;
	std::stack<int> CallStack;
	bool Halted = false;
	TWindow* Wnd = nullptr;

	Machine(const Machine& other) = delete;

	void InitOpcodes();
	const byte& NextByte();
	const int NextWord();
	void Abort(std::string msg);
	void Execute(byte opcode);
	int PopNumber();
};
