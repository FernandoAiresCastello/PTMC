#include "Machine.h"
#include "Command.h"

//=============================================================================
//	Command implementation
//=============================================================================

void Machine::O_Nop()
{
	// No operation
}

void Machine::O_PushIntConst()
{
	ParamStack.push(NextProgramInt());
}

void Machine::O_PushIntArrayConst()
{
	for (auto& val : NextProgramIntArray())
		ParamStack.push(val);
}

void Machine::O_Pop()
{
	ParamStack.pop();
}

void Machine::O_Goto()
{
	ExecPtr = NextProgramUint();
}

void Machine::O_Pause()
{
	PauseCycles = Pop();
}

void Machine::O_GfxCreate()
{
	if (Wnd != nullptr) {
		Abort("Window is already open");
		return;
	}

	int fullscreen = Pop();
	int zoom = Pop();
	int height = Pop();
	int width = Pop();

	Wnd = new TWindow(width, height, zoom, fullscreen);
}

void Machine::O_GfxUpdate()
{
	Wnd->Update();
}

void Machine::O_GfxClear()
{
	Wnd->Clear(Pal, BackColor);
}

void Machine::O_GfxBackColorSet()
{
	BackColor = Pop();
}

void Machine::O_GfxTitleSet()
{
	AbortCommandNotImplemented();
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
//	Basic functions
//=============================================================================

Machine::Machine()
{
}

Machine::~Machine()
{
	delete[] ProgMemory;
	delete[] DataMemory;
}

void Machine::Run(Program* prog)
{
	if (Running) {
		Abort("Machine is already running");
		return;
	}

	DataMemorySize = 0x10000;
	DataMemory = new int[DataMemorySize];

	ProgMemorySize = prog->Bytecode.size();
	ProgMemory = new byte[ProgMemorySize];
	for (int i = 0; i < ProgMemorySize; i++)
		ProgMemory[i] = prog->Bytecode[i];

	ExecPtr = 0;
	Running = true;
	Halted = false;

	while (Running) {
		SDL_Event e = { 0 };
		SDL_PollEvent(&e);
		HandleGlobalEvents(e);

		if (PauseCycles > 0) {
			PauseCycles--;
			SDL_Delay(1);
			continue;
		}

		if (Halted)
			continue;

		if (ExecPtr < ProgMemorySize)
			Execute(NextProgramByte());
		else
			Abort("Execution pointer past end of program");

		if (!Running)
			break;
	}
}

bool Machine::HandleGlobalEvents(SDL_Event& e)
{
	if (e.type == SDL_QUIT) {
		Running = false;
		return true;
	}
	else if (e.type == SDL_KEYDOWN) {
		const SDL_Keycode key = e.key.keysym.sym;
		if (key == SDLK_PAUSE) {
			Running = false;
			return true;
		}
		else if (key == SDLK_RETURN && TKey::Alt() && Wnd != nullptr) {
			Wnd->ToggleFullscreen();
			Wnd->Update();
			return true;
		}
	}

	return false;
}

byte& Machine::NextProgramByte()
{
	if (ExecPtr < ProgMemorySize)
		return ProgMemory[ExecPtr++];

	return NullOpcode;
}

int Machine::NextProgramInt()
{
	byte& sign = NextProgramByte();
	byte& hi = NextProgramByte();
	byte& lo = NextProgramByte();
	byte nibbles[2] = { lo, hi };
	ushort value = Util::BytesToShort(nibbles);

	return sign == 0 ? value : -value;
}

int Machine::NextProgramUint()
{
	byte& hi = NextProgramByte();
	byte& lo = NextProgramByte();
	byte nibbles[2] = { lo, hi };
	return Util::BytesToShort(nibbles);
}

std::vector<int> Machine::NextProgramIntArray()
{
	std::vector<int> array;
	byte& count = NextProgramByte();
	for (int i = 0; i < count; i++)
		array.push_back(NextProgramInt());
	
	return array;
}

void Machine::Abort(std::string msg)
{
	Running = false;

	MsgBox::Error(String::Format("Runtime error at program index %i (0x%x):\n\n%s", 
		ExecPtr, ExecPtr, msg.c_str()));
}

void Machine::AbortCommandNotImplemented()
{
	Abort("Command not implemented");
}

void Machine::Execute(byte opcode)
{
	auto fn = Command::Impl[opcode];
	if (fn) {
		(this->*fn)();
	}
	else {
		Abort(String::Format("Invalid command: 0x%02x", opcode));
	}
}

int Machine::Pop()
{
	if (ParamStack.empty()) {
		Abort("Stack empty");
		return 0;
	}

	int& top = ParamStack.top();
	ParamStack.pop();
	return top;
}
