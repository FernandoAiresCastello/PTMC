#include "Machine.h"
#include "Command.h"

void Machine::O_Nop()
{
	// No operation
}

void Machine::O_PushByte()
{
	ParamStack.push(NextByte());
}

void Machine::O_PushWord()
{
	ParamStack.push(NextWord());
}

void Machine::O_Goto()
{
	ProgramPtr = NextWord();
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

void Machine::O_WindowUpdate()
{
	Wnd->Update();
}

void Machine::O_WindowClear()
{
	Wnd->Clear(Pal, BackColor);
}

void Machine::O_WindowBackColorSet()
{
	BackColor = PopNumber();
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
	Memory = new int[MemorySize];
}

Machine::~Machine()
{
	delete[] Memory;
}

void Machine::Run(Program* prog)
{
	if (Running) {
		Abort("Machine is already running");
		return;
	}

	Prog = prog;
	ProgramPtr = 0;
	Running = true;
	Halted = false;

	while (Running) {
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
				Wnd->Update();
			}
		}

		if (Halted)
			continue;

		if (ProgramPtr < Prog->Bytecode.size())
			Execute(NextByte());
		else
			Abort("Execution pointer past end of program");

		if (!Running)
			break;
	}
}

const byte& Machine::NextByte()
{
	if (ProgramPtr < Prog->Bytecode.size())
		return Prog->Bytecode[ProgramPtr++];

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
	auto fn = Command::Impl[opcode];
	if (fn) {
		(this->*fn)();
	}
	else {
		Abort(TString::Format("Invalid opcode 0x%02x", opcode));
	}
}

int Machine::PopNumber()
{
	if (ParamStack.empty()) {
		Abort("Stack empty");
		return 0;
	}

	int& top = ParamStack.top();
	ParamStack.pop();
	return top;
}
