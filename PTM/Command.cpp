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
		OP("PUSH", 0x10, O_PushByte);
		OP("PUSHW", 0x11, O_PushWord);
		// ===[ Graphics ]===
		OP("WND.OPEN", 0x20, O_WindowCreate);
		// ===[ Debugging ]===
		OP("BREAK", 0xf0, O_Break);
		// ===[ System ]===
		OP("HALT", 0xfe, O_Halt);
		OP("EXIT", 0xff, O_Exit);
	}
}
