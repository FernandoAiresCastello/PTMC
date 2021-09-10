#pragma once
#include "Program.h"
#include <map>
#include <vector>
#include <string>
#include <stack>
#include <TileGameLib.h>
#include <CppUtils/CppUtils.h>

using namespace TileGameLib;
using namespace CppUtils;

struct StringLiteral;

class Machine
{
public:
	// Opcodes
	void O_Nop();
	void O_PushIntConst();
	void O_PushIntArrayConst();
	void O_PushStringConst();
	void O_Pop();
	void O_StoreIntDirect();
	void O_StoreIntIndirect();
	void O_StoreString();
	void O_LoadIntDirect();
	void O_LoadIntIndirect();
	void O_Goto();
	void O_Pause();
	void O_GfxCreate();
	void O_GfxUpdate();
	void O_GfxClear();
	void O_GfxBackColorSet();
	void O_GfxTitleSet();
	void O_Break();
	void O_Halt();
	void O_Exit();

public:
	Machine();
	~Machine();

	void Run(Program* prog);

private:
	byte NullOpcode = 0;
	bool Running = false;
	int ExecPtr = 0;
	byte* ProgMemory = nullptr;
	int ProgMemorySize = 0;
	int* DataMemory = nullptr;
	int DataMemorySize = 0;
	std::stack<int> ParamStack;
	std::stack<int> CallStack;
	bool Halted = false;
	TWindow* Wnd = nullptr;
	TPalette* Pal = TPalette::Default;
	TCharset* Chr = TCharset::Default;
	int BackColor = 0;
	unsigned long PauseCycles = 0;

	void InitDefaultPalette();
	bool HandleGlobalEvents(SDL_Event& e);
	void ClearMemory();
	byte& NextProgramByte();
	int NextProgramInt();
	int NextProgramUint();
	std::vector<int> NextProgramIntArray();
	StringLiteral NextProgramStringLiteral();
	void Abort(std::string msg);
	void Execute(byte opcode);
	int Pop();
	std::vector<int> GetValuesInMemoryRegion(int firstAddr, int lastAddr);
	void DumpMemoryToFile(std::string filename, int firstAddr, int lastAddr);
	std::string GetStringFromMemory(int ptr);
};
