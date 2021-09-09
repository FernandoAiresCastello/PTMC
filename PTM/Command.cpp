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
		OP("POP", 0x12, O_Pop);

		// ===[ Control flow ]===
		OP("GOTO", 0x20, O_Goto);
		OP("PAUSE", 0x21, O_Pause);
		
		// ===[ Graphics ]===
		OP("GFX.OPEN", 0x30, O_GfxCreate);
		OP("GFX.UPDATE", 0x31, O_GfxUpdate);
		OP("GFX.CLEAR", 0x32, O_GfxClear);
		OP("GFX.BGC.SET", 0x33, O_GfxBackColorSet);
		OP("GFX.TITLE.SET", 0x34, O_GfxTitleSet);
		
		// ===[ Debugging ]===
		OP("BREAK", 0xf0, O_Break);
		
		// ===[ System ]===
		OP("HALT", 0xfe, O_Halt);
		OP("EXIT", 0xff, O_Exit);
	}
}
