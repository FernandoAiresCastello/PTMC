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
	std::map<std::string, std::string> Defs;
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

#define POP Machine.Stack.top(); Machine.Stack.pop()
#define PUSH(x) Machine.Stack.push(x)

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

void CompileAndRunProgram(Program* prog)
{
	RestartTileMachine();

	int actualLineIndex = 0;

	for (int i = 0; i < prog->Lines.size(); i++) {
		ProgramLine* line = prog->Lines[i];
		auto parts = String::Split(line->Command, ' ');
		
		// skip empty lines
		if (parts.empty()) {
			continue;
		}
		std::string command = parts[0];
		if (command.empty()) {
			continue;
		}
		// skip comments
		if (String::StartsWith(command, '#')) {
			continue;
		}
		// collect then skip labels
		if (String::EndsWith(command, ':')) {
			std::string label = String::RemoveLast(command);
			Machine.ProgramLabels[label] = actualLineIndex;
			continue;
		}
		// collect then skip defs
		if (String::StartsWith(command, ".def")) {
			Machine.Defs[parts[1]] = parts[2];
			continue;
		}

		command = String::ToUpper(command);
		int commandCode = GetCommandByteCode(command);
		int paramNumber = 0;
		std::string paramString = "";
		bool hasParam = false;

		if (parts.size() > 1) {
			hasParam = true;
			std::string param = parts[1];
			if (Machine.Defs.find(param) != Machine.Defs.end()) {
				param = Machine.Defs[param];
			}
			paramString = param;
			paramNumber = String::ToInt(param);
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
	if (command == "BRK") return 0xfd;
	if (command == "HALT") return 0xfe;
	if (command == "EXIT") return 0xff;
	if (command == "PUSH") return 0x01;
	if (command == "POP") return 0x02;
	if (command == "POKE") return 0x03;
	if (command == "PEEK") return 0x04;
	if (command == "PUTC") return 0x05;
	if (command == "REFR") return 0x06;
	if (command == "ADD") return 0x07;
	if (command == "SUB") return 0x08;
	if (command == "JP") return 0x09;
	if (command == "WAIT") return 0x0a;

	return -1;
}

void ExecuteCompiledProgramLine(CompiledProgramLine& line)
{
	switch (line.Command) {

		case 0x00: C_Nop(); break;
		case 0x01: C_Push(); break;
		case 0x02: C_Pop(); break;
		case 0x03: C_Mset(); break;
		case 0x04: C_Mget(); break;
		case 0x05: C_Putc(); break;
		case 0x06: C_Refr(); break;
		case 0x07: C_Add(); break;
		case 0x08: C_Sub(); break;
		case 0xfd: C_Brk(); break;
		case 0xfe: C_Halt(); break;
		case 0xff: C_Exit(); break;
		case 0x09: C_Jp(); break;
		case 0x0a: C_Wait(); break;
		
		default:
			SetError(ERR_INVALID_COMMAND);
			break;
	}
}

void SetError(std::string error)
{
	Machine.Error = String::Format("%i:%s", Machine.Line->SourceLineNumber, error.c_str());
}

void BranchTo(std::string label)
{
	Machine.Branching = true;
	Machine.ProgramPtr = Machine.ProgramLabels[label];
}

void C_Nop()
{
	if (Machine.Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	// no operation
}

void C_Push()
{
	if (!Machine.Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	PUSH(Machine.Line->ParamNumber);
}

void C_Pop()
{
	if (Machine.Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}
	if (Machine.Stack.empty()) {
		SetError(ERR_STACK_EMPTY);
		return;
	}

	Machine.Stack.pop();
}

void C_Mset()
{
	if (!Machine.Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}
	if (Machine.Line->ParamNumber < 0 || Machine.Line->ParamNumber >= Machine.MemorySize) {
		SetError(ERR_ILLEGAL_ADDRESS);
		return;
	}
	if (Machine.Stack.empty()) {
		SetError(ERR_STACK_EMPTY);
		return;
	}

	Machine.Memory[Machine.Line->ParamNumber] = POP;
}

void C_Mget()
{
	if (!Machine.Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}
	if (Machine.Line->ParamNumber < 0 || Machine.Line->ParamNumber >= Machine.MemorySize) {
		SetError(ERR_ILLEGAL_ADDRESS);
		return;
	}

	PUSH(Machine.Memory[Machine.Line->ParamNumber]);
}

void C_Brk()
{
	if (Machine.Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	__debugbreak();
}

void C_Halt()
{
	if (Machine.Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	Machine.Halted = true;
}

void C_Exit()
{
	if (Machine.Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	Machine.Running = false;
}

void C_Putc()
{
	if (Machine.Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}
	if (Machine.Stack.size() < 5) {
		SetError(ERR_STACK_EMPTY);
		return;
	}

	int bgc = POP;
	int fgc = POP;
	int ch = POP;
	int y = POP;
	int x = POP;
	
	PutChar(ch, x, y, fgc, bgc);
}

void C_Refr()
{
	if (Machine.Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	RefreshScreen();
}

void C_Add()
{
	if (Machine.Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}
	if (Machine.Stack.size() < 2) {
		SetError(ERR_STACK_EMPTY);
		return;
	}

	int b = POP;
	int a = POP;
	PUSH(a + b);
}

void C_Sub()
{
	if (Machine.Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}
	if (Machine.Stack.size() < 2) {
		SetError(ERR_STACK_EMPTY);
		return;
	}

	int b = POP;
	int a = POP;
	PUSH(a - b);
}

void C_Jp()
{
	if (!Machine.Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	BranchTo(Machine.Line->ParamString);
}

void C_Wait()
{
	if (!Machine.Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	Util::Pause(Machine.Line->ParamNumber);
}
