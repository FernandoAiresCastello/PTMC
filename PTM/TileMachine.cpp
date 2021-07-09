#include <map>
#include <stack>
#include "TileMachine.h"
#include "SharedScreen.h"
#include "Program.h"

struct {
	
	int* Memory = NULL;
	const int MemorySize = 65536;
	std::string Error = "";
	std::vector<CompiledProgramLine> Program;
	int ProgramPtr = 0;
	CompiledProgramLine* Line = NULL;
	std::map<std::string, int> ProgramLabels;
	bool Branching = false;
	bool Running = false;
	bool Halted = false;
	std::stack<int> Stack;

} Machine;

#define ERR_PROGRAM_EMPTY "Program empty"
#define ERR_SYNTAX_ERROR "Syntax error"
#define ERR_INVALID_COMMAND "Invalid command"
#define ERR_END_OF_PROGRAM "End of program"
#define ERR_ILLEGAL_ADDRESS "Illegal address"
#define ERR_STACK_EMPTY "Stack empty"

void InitTileMachine()
{
	Machine.Memory = new int[Machine.MemorySize];
}

void DestroyTileMachine()
{
	delete[] Machine.Memory;
}

void RestartTileMachine()
{
	Machine.Program.clear();
	Machine.ProgramLabels.clear();
	Machine.ProgramPtr = 0;
	Machine.Error = "";
	Machine.Running = true;
	Machine.Halted = false;
	while (!Machine.Stack.empty()) {
		Machine.Stack.pop();
	}
}

std::string GetTileMachineError()
{
	return Machine.Error;
}

void RefreshScreen()
{
	DrawScreenBorder();
	DrawScreenBuffer();
	UpdateEntireScreen();
}

void RunProgram(Program* prog)
{
	RestartTileMachine();

	int actualLineIndex = 0;

	for (int i = 0; i < prog->Lines.size(); i++) {
		ProgramLine* line = prog->Lines[i];
		std::vector<std::string> parts = String::Split(line->Command, ' ');
		if (parts.empty()) {
			continue;
		}
		std::string command = parts[0];
		if (command.empty()) {
			continue;
		}

		if (String::EndsWith(command, ':')) {
			std::string label = String::RemoveLast(command);
			Machine.ProgramLabels[label] = actualLineIndex;
			continue;
		}

		command = String::ToUpper(command);
		int commandCode = GetCommandByteCode(command);
		int paramNumber = 0;
		std::string paramString = "";
		bool hasParam = false;

		if (parts.size() > 1) {
			hasParam = true;
			paramString = parts[1];
			paramNumber = String::ToInt(parts[1]);
		}

		CompiledProgramLine cl;
		cl.SourceLineNumber = i + 1;
		cl.ActualLineIndex = actualLineIndex;
		cl.Command = commandCode;
		cl.HasParam = hasParam;
		cl.ParamNumber = paramNumber;
		cl.ParamString = paramString;

		Machine.Program.push_back(cl);

		actualLineIndex++;
	}

	ExecuteCompiledProgram();
}

void ExecuteCompiledProgram()
{
	if (Machine.Program.empty()) {
		Machine.Error = ERR_PROGRAM_EMPTY;
		return;
	}

	ClearScreen();
	RefreshScreen();
	
	while (Machine.Running) {

		SDL_Event e = { 0 };
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			Machine.Running = false;
			continue;
		}
		else if (e.type == SDL_KEYDOWN) {
			SDL_Keycode key = e.key.keysym.sym;
			if (key == SDLK_ESCAPE) {
				Machine.Running = false;
				continue;
			}
		}

		if (Machine.Halted)
			continue;

		Machine.Branching = false;

		Machine.Line = &Machine.Program[Machine.ProgramPtr];
		ExecuteCompiledProgramLine(Machine.Program[Machine.ProgramPtr]);

		if (!Machine.Error.empty()) {
			Machine.Running = false;
			continue;
		}
		if (Machine.Running && !Machine.Halted && !Machine.Branching) {
			Machine.ProgramPtr++;
		}
		if (Machine.ProgramPtr >= Machine.Program.size()) {
			Machine.Running = false;
			Machine.Error = String::Format("%i:" ERR_END_OF_PROGRAM, 
				Machine.Program[Machine.Program.size() - 1].SourceLineNumber + 1);
		}
	}
}

int GetCommandByteCode(std::string& command)
{
	if (command == "NOP") return 0x00;
	if (command == "PUSH") return 0x01;
	if (command == "POP") return 0x02;
	if (command == "MSET") return 0x03;
	if (command == "MGET") return 0x04;
	if (command == "PUTC") return 0x05;
	if (command == "REFR") return 0x06;
	if (command == "HALT") return 0xfe;
	if (command == "EXIT") return 0xff;

	return -1;
}

void ExecuteCompiledProgramLine(CompiledProgramLine& line)
{
	switch (line.Command) {

		case 0x00: Cmd_Nop(line); break;
		case 0x01: Cmd_Push(line); break;
		case 0x02: Cmd_Pop(line); break;
		case 0x03: Cmd_Mset(line); break;
		case 0x04: Cmd_Mget(line); break;
		case 0x05: Cmd_Putc(line); break;
		case 0x06: Cmd_Refr(line); break;
		case 0xfe: Cmd_Halt(line); break;
		case 0xff: Cmd_Exit(line); break;
		
		default:
			SetError(ERR_INVALID_COMMAND);
			break;
	}
}

void SetError(std::string error)
{
	Machine.Error = String::Format("%i:%s", Machine.Line->SourceLineNumber, error.c_str());
}

void Cmd_Nop(CompiledProgramLine& line)
{
	if (line.HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	// no operation
}

void Cmd_Push(CompiledProgramLine& line)
{
	if (!line.HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	Machine.Stack.push(line.ParamNumber);
}

void Cmd_Pop(CompiledProgramLine& line)
{
	if (line.HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}
	if (Machine.Stack.empty()) {
		SetError(ERR_STACK_EMPTY);
		return;
	}

	Machine.Stack.pop();
}

void Cmd_Mset(CompiledProgramLine& line)
{
	if (!line.HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}
	if (line.ParamNumber < 0 || line.ParamNumber >= Machine.MemorySize) {
		SetError(ERR_ILLEGAL_ADDRESS);
		return;
	}
	if (Machine.Stack.empty()) {
		SetError(ERR_STACK_EMPTY);
		return;
	}

	Machine.Memory[line.ParamNumber] = Machine.Stack.top();
	Machine.Stack.pop();
}

void Cmd_Mget(CompiledProgramLine& line)
{
	if (!line.HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}
	if (line.ParamNumber < 0 || line.ParamNumber >= Machine.MemorySize) {
		SetError(ERR_ILLEGAL_ADDRESS);
		return;
	}

	Machine.Stack.push(Machine.Memory[line.ParamNumber]);
}

void Cmd_Halt(CompiledProgramLine& line)
{
	if (line.HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	Machine.Halted = true;
}

void Cmd_Exit(CompiledProgramLine& line)
{
	if (line.HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	Machine.Running = false;
}

void Cmd_Putc(CompiledProgramLine& line)
{
	if (line.HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}
	if (Machine.Stack.size() < 5) {
		SetError(ERR_STACK_EMPTY);
		return;
	}

	static int bgc = Machine.Stack.top(); Machine.Stack.pop();
	static int fgc = Machine.Stack.top(); Machine.Stack.pop();
	static int ch = Machine.Stack.top(); Machine.Stack.pop();
	static int y = Machine.Stack.top(); Machine.Stack.pop();
	static int x = Machine.Stack.top(); Machine.Stack.pop();
	
	PutChar(ch, x, y, fgc, bgc);
}

void Cmd_Refr(CompiledProgramLine& line)
{
	if (line.HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	RefreshScreen();
}
