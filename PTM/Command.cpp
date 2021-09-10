#include "Command.h"

#define OP(name,opcode,fn) Impl[opcode] = &Machine::fn; Name[name] = opcode;

namespace Command
{
	std::map<byte, void(Machine::*)()> Impl;
	std::map<std::string, byte> Name;

	void Init()
	{
		// ===[ Special ]===
		OP("NOP", 0x00, O_Nop);

		// ===[ Param stack ]===
		OP("PUSH", 0x10, O_PushIntConst);
		OP("PUSHA", 0x11, O_PushIntArrayConst);
		OP("PUSHS", 0x12, O_PushStringConst);
		OP("POP", 0x13, O_Pop);

		// ===[ Memory ]===
		OP("STORE", 0x20, O_StoreIntDirect);
		OP("STORE.I", 0x21, O_StoreIntIndirect);
		OP("STORE.S", 0x22, O_StoreString);
		OP("LOAD", 0x23, O_LoadIntDirect);
		OP("LOAD.I", 0x24, O_LoadIntIndirect);

		// ===[ Control flow ]===
		OP("GOTO", 0x30, O_Goto);
		OP("CALL", 0x31, O_Call);
		OP("RET", 0x32, O_Return);
		OP("PAUSE", 0x33, O_Pause);
		
		// ===[ Graphics ]===
		OP("GFX.OPEN", 0x40, O_GfxCreate);
		OP("GFX.UPDATE", 0x41, O_GfxUpdate);
		OP("GFX.CLEAR", 0x42, O_GfxClear);
		OP("GFX.BGC.SET", 0x43, O_GfxBackColorSet);
		OP("GFX.TITLE.SET", 0x44, O_GfxTitleSet);
		
		// ===[ Debugging ]===
		OP("BREAK", 0xf0, O_Break);
		
		// ===[ System ]===
		OP("HALT", 0xfe, O_Halt);
		OP("EXIT", 0xff, O_Exit);
	}
}
