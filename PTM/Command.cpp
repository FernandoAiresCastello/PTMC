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
		OP("PUSHB", 0x12, O_PushByteArrayConst);
		OP("PUSHS", 0x13, O_PushStringConst);
		OP("POP", 0x14, O_Pop);
		OP("POPA", 0x15, O_PopAll);

		// ===[ Memory ]===
		OP("STORE", 0x20, O_StoreIntDirect);
		OP("STORE.I", 0x21, O_StoreIntIndirect);
		OP("STORE.S", 0x22, O_StoreString);
		OP("LOAD", 0x23, O_LoadIntDirect);
		OP("LOAD.I", 0x24, O_LoadIntIndirect);

		// ===[ Control flow ]===
		OP("GOTO", 0x30, O_Goto);
		OP("GOTONZ", 0x31, O_GotoIfNotZero);
		OP("CALL", 0x32, O_Call);
		OP("RET", 0x33, O_Return);
		OP("PAUSE", 0x34, O_Pause);
		
		// ===[ Graphics ]===
		OP("GFX.OPEN", 0x40, O_GfxCreate);
		OP("GFX.UPDATE", 0x41, O_GfxUpdate);
		OP("GFX.CLEAR", 0x42, O_GfxClear);
		OP("GFX.BGC.SET", 0x43, O_GfxBackColorSet);
		OP("GFX.TITLE.SET", 0x44, O_GfxTitleSet);
		OP("GFX.PAL.SET", 0x45, O_GfxPaletteSet);
		OP("GFX.PUT", 0x46, O_GfxPut);
		
		// ===[ Arithmetic ]===
		OP("INC", 0x50, O_Increment);
		OP("DEC", 0x51, O_Decrement);

		// ===[ Utils ]===
		OP("RND.GET", 0xe0, O_PushRandom);

		// ===[ Debugging ]===
		OP("DEBUG.ON", 0xf0, O_DebugOn);
		OP("DEBUG.OFF", 0xf1, O_DebugOff);
		OP("DEBUG.BRK", 0xf2, O_DebugBreak);
		OP("DEBUG.PSTK", 0xf3, O_DebugParamStackDump);
		OP("DEBUG.CSTK", 0xf4, O_DebugCallStackDump);
		OP("DEBUG.MEM", 0xf5, O_DebugMemoryDump);
		
		// ===[ System ]===
		OP("HALT", 0xfe, O_Halt);
		OP("EXIT", 0xff, O_Exit);
	}
}
