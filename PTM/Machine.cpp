#include <sstream>
#include "Machine.h"
#include "Command.h"

struct StringLiteral {
	int Length;
	std::vector<byte> Chars;
};

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

void Machine::O_PushByteArrayConst()
{
	for (auto& val : NextProgramByteArray())
		ParamStack.push(val);
}

void Machine::O_PushStringConst()
{
	StringLiteral str = NextProgramStringLiteral();
	for (auto& val : str.Chars)
		ParamStack.push(val);

	ParamStack.push(str.Length);
}

void Machine::O_Pop()
{
	ParamStack.pop();
}

void Machine::O_PopAll()
{
	while (!ParamStack.empty())
		ParamStack.pop();
}

void Machine::O_StoreIntDirect()
{
	DataMemory[NextProgramUint()] = Pop();
}

void Machine::O_StoreIntIndirect()
{
	int ptr = NextProgramUint();
	int addr = DataMemory[ptr];
	int value = Pop();
	DataMemory[addr] = value;
}

void Machine::O_StoreString()
{
	int addr = NextProgramUint();
	int length = Pop();
	for (int i = 0; i < length; i++)
		DataMemory[addr++] = Pop();

	DataMemory[addr] = 0;
}

void Machine::O_LoadIntDirect()
{
	ParamStack.push(DataMemory[NextProgramUint()]);
}

void Machine::O_LoadIntIndirect()
{
	int ptr = NextProgramUint();
	int addr = DataMemory[ptr];
	int value = DataMemory[addr];
	ParamStack.push(value);
}

void Machine::O_Goto()
{
	ExecPtr = NextProgramUint();
}

void Machine::O_Call()
{
	Call();
}

void Machine::O_Return()
{
	Return();
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
	int paletteIndex = Pop();
	if (paletteIndex < 0 || paletteIndex >= Pal->GetSize()) {
		Abort("Parameter value out of range");
		return;
	}

	BackColor = paletteIndex;
}

void Machine::O_GfxTitleSet()
{
	Wnd->SetTitle(GetStringFromMemory(Pop()));
}

void Machine::O_GfxPaletteSet()
{
	int b = Pop();
	int g = Pop();
	int r = Pop();
	int ix = Pop();

	Pal->Set(ix, r, g, b);
}

void Machine::O_GfxPut()
{
	int bgc = Pop();
	int fgc = Pop();
	int ch = Pop();
	int y = Pop();
	int x = Pop();
	
	Wnd->DrawChar(Chr, Pal, ch, fgc, bgc, x, y);
}

void Machine::O_Increment()
{
	int value = Pop();
	value++;
	ParamStack.push(value);
}

void Machine::O_Decrement()
{
	int value = Pop();
	value--;
	ParamStack.push(value);
}

void Machine::O_PushRandom()
{
	int max = Pop();
	int min = Pop();
	ParamStack.push(Util::Random(min, max));
}

void Machine::O_DebugOn()
{
	DebugEnabled = true;
}

void Machine::O_DebugOff()
{
	DebugEnabled = false;
}

void Machine::O_DebugBreak()
{
	if (!DebugEnabled)
		return;
	
	MessageBox(String::Format("Breakpoint at program index %i (0x%x)", ExecPtr));
}

void Machine::O_DebugMsgBoxShow()
{
}

void Machine::O_DebugParamStackDump()
{
	if (!DebugEnabled)
		return;

	std::string dump = "Parameter stack:\n\n";
	dump.append(StackToString(ParamStack));
	MessageBox(dump);
}

void Machine::O_DebugCallStackDump()
{
	if (!DebugEnabled)
		return;

	std::string dump = "Call stack:\n\n";
	dump.append(StackToString(CallStack));
	MessageBox(dump);
}

void Machine::O_DebugMemoryDump()
{
	if (!DebugEnabled)
		return;

	std::string dump = "Memory:\n\n";
	int lastAddr = Pop();
	int firstAddr = Pop();
	dump.append(MemoryToString(firstAddr, lastAddr));
	MessageBox(dump);
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
	Util::Randomize();
	InitDefaultPalette();
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
	ClearMemory();

	ProgMemorySize = prog->Bytecode.size();
	if (ProgMemorySize > 65535) {
		Abort("Invalid bytecode size");
		return;
	}
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
		if (key == SDLK_RETURN && TKey::Alt() && Wnd != nullptr) {
			Wnd->ToggleFullscreen();
			Wnd->Update();
			return true;
		}
	}

	return false;
}

void Machine::ClearMemory()
{
	for (int i = 0; i < DataMemorySize; i++)
		DataMemory[i] = 0;
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

std::vector<byte> Machine::NextProgramByteArray()
{
	std::vector<byte> array;
	byte& count = NextProgramByte();
	for (int i = 0; i < count; i++)
		array.push_back(NextProgramByte());

	return array;
}

StringLiteral Machine::NextProgramStringLiteral()
{
	StringLiteral str;
	str.Length = NextProgramByte();
	for (int i = 0; i < str.Length; i++)
		str.Chars.push_back(NextProgramByte());

	return str;
}

void Machine::Abort(std::string msg)
{
	Running = false;
	MsgBox::Error(String::Format("Runtime error at program index %i (0x%x):\n\n%s",
		ExecPtr, ExecPtr, msg.c_str()));
}

void Machine::MessageBox(std::string msg)
{
	MessageBox("PTM", msg);
}

void Machine::MessageBox(std::string title, std::string msg)
{
	MsgBox::Info(title, msg);
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

std::vector<int> Machine::GetValuesInMemoryRegion(int firstAddr, int lastAddr)
{
	std::vector<int> mem;
	for (int addr = firstAddr; addr <= lastAddr; addr++)
		mem.push_back(DataMemory[addr]);

	return mem;
}

void Machine::DumpMemoryToFile(std::string filename, int firstAddr, int lastAddr)
{
	std::string dec;
	std::string hex;
	std::string ascii;
	std::vector<std::string> dump;

	auto mem = GetValuesInMemoryRegion(firstAddr, lastAddr);
	for (auto& val : mem) {
		dec += String::Format("%i ", val);
		hex += String::Format("%04X ", val);
		ascii += (val >= 0x20 && val < 0x7f) ? (char)val : '.';
	}

	dump.push_back("PTM Memory Dump");
	dump.push_back(String::Format("Datetime: %s", Util::DateTime().c_str()));
	dump.push_back(String::Format("Address: 0x%04X to 0x%04X", firstAddr, lastAddr));
	dump.push_back("");
	dump.push_back(dec);
	dump.push_back(hex);
	dump.push_back(ascii);

	TFile::WriteLines(filename, dump);
}

std::string Machine::StackToString(std::stack<int>& stk)
{
	std::stringstream ss;
	std::stack<int> params = stk;
	for (int i = 0; i < stk.size(); i++) {
		int value = params.top();
		char ch = (value >= 0x20 && value < 0x7f) ? (char)value : '.';
		params.pop();
		ss << value << " / 0x" << 
			String::IntToHex(value, true) << " / " << ch << "\n";
	}

	std::string str = ss.str();
	return str.empty() ? "(Empty)" : str;
}

std::string Machine::MemoryToString(int firstAddr, int lastAddr)
{
	std::stringstream ss;
	auto values = GetValuesInMemoryRegion(firstAddr, lastAddr);
	for (int i = 0; i < values.size(); i++) {
		int value = values[i];
		char ch = (value >= 0x20 && value < 0x7f) ? (char)value : '.';
		ss << String::Format("[0x%04X] ", firstAddr + i) <<
			value << " / 0x" << 
			String::IntToHex(values[i], true) <<  " / " << ch << "\n";
	}

	return ss.str();
}

std::string Machine::GetStringFromMemory(int ptr)
{
	std::vector<int> mem;
	int ch = -1;

	while (ch != 0) {
		ch = DataMemory[ptr++];
		mem.push_back(ch);
	}

	return std::string(mem.begin(), mem.end());
}

void Machine::Call()
{
	CallStack.push(ExecPtr + 2);
	ExecPtr = NextProgramUint();
}

void Machine::Return()
{
	if (CallStack.empty()) {
		Abort("Call stack empty");
		return;
	}

	ExecPtr = CallStack.top();
	CallStack.pop();
}

void Machine::InitDefaultPalette()
{
	Pal->Clear();
	Pal->Set(0, 0x000000);
	Pal->Set(1, 0xffffff);
}
