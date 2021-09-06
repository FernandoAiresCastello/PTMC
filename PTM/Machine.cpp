#include "Machine.h"

std::map<std::string, byte> CmdMap;

void Machine::InitOpcodes()
{
	// ===[ Special ]===
	OP("NOP", 0x00, O_Nop);
	// ===[ Param stack ]===
	OP("PUSH", 0x10, O_Push);
	// ===[ Graphics ]===
	OP("WND.OPEN", 0x20, O_WindowCreate);
	// ===[ Debugging ]===
	OP("BREAK", 0xf0, O_Break);
	// ===[ System ]===
	OP("HALT", 0xfe, O_Halt);
	OP("EXIT", 0xff, O_Exit);
}

void Machine::O_Nop()
{
	// No operation
}

void Machine::O_Push()
{
	ParamStack.push(NextWord());
}

void Machine::O_WindowCreate()
{
	if (Wnd != nullptr) {
		Abort("Window is already open");
		return;
	}

	int fullscreen = PopNumber();
	int zoom = PopNumber();
	int height = PopNumber();
	int width = PopNumber();

	Wnd = new TWindow(width, height, zoom, fullscreen);
}

void Machine::O_Break()
{
	SDL_TriggerBreakpoint();
}

void Machine::O_Halt()
{
	Halted = true;
}

void Machine::O_Exit()
{
	Running = false;
}

//=============================================================================

Machine::Machine()
{
	InitOpcodes();

	Memory = new int[MemorySize];
}

Machine::~Machine()
{
	delete[] Memory;
}

void Machine::Run(std::vector<byte>& bytecode)
{
	if (Running) {
		Abort("Machine is already running");
		return;
	}

	Bytecode = bytecode;
	ProgramPtr = 0;
	Running = true;
	Halted = false;

	while (Running) {
		if (Wnd) {
			Wnd->Update();
		}

		SDL_Event e = { 0 };
		SDL_PollEvent(&e);

		if (e.type == SDL_QUIT) {
			Running = false;
			break;
		}
		else if (e.type == SDL_KEYDOWN) {
			const SDL_Keycode key = e.key.keysym.sym;
			if (key == SDLK_PAUSE) {
				Running = false;
				break;
			}
			else if (key == SDLK_RETURN && TKey::Alt() && Wnd != nullptr) {
				Wnd->ToggleFullscreen();
			}
		}

		if (Halted)
			continue;

		if (ProgramPtr < bytecode.size())
			Execute(NextByte());
		else
			Abort("Execution pointer past end of program");

		if (!Running)
			break;
	}
}

const byte& Machine::NextByte()
{
	if (ProgramPtr < Bytecode.size())
		return Bytecode[ProgramPtr++];

	return NullOpcode;
}

const int Machine::NextWord()
{
	const byte& hi = NextByte();
	const byte& lo = NextByte();
	byte nibbles[2] = { lo, hi };
	return TUtil::BytesToShort(nibbles);
}

void Machine::Abort(std::string msg)
{
	Running = false;

	TUtil::Error(TString::Format("Runtime error at program index %i (0x%x):\n\n%s", 
		ProgramPtr, ProgramPtr, msg.c_str()));
}

void Machine::Execute(byte opcode)
{
	auto fn = Opcodes[opcode];
	if (fn) {
		(this->*fn)();
	}
	else {
		Abort(TString::Format("Invalid opcode 0x%02x", opcode));
	}
}

int Machine::PopNumber()
{
	int& top = ParamStack.top();
	ParamStack.pop();
	return top;
}
