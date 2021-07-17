#include "Machine.h"

#define ERR_END_OF_PROGRAM "Execution pointer past end of program"
#define ERR_INVALID_COMMAND "Invalid command"
#define ERR_SYNTAX_ERROR "Syntax error"
#define ERR_STACK_EMPTY "Stack empty"

Machine::Machine()
{
	Running = false;
	ProgramPtr = 0;
	Branch = false;
	Error = "";

	InitCommandMap();
}

Machine::~Machine()
{
}

void Machine::Run(Datafile* data)
{
	Program* prog = data->GetProgram();

	Running = true;
	while (Running) {
		
		Error = "";

		if (ProgramPtr >= prog->GetSize()) {
			SetError(ERR_END_OF_PROGRAM);
			break;
		}

		Line = prog->GetLine(ProgramPtr);
		auto& impl = Cmd[Line->Command];
		if (impl != NULL) {
			((*this).*impl)();
		}
		else {
			SetError(ERR_INVALID_COMMAND);
			break;
		}

		if (!Error.empty())
			break;
		if (Branch)
			Branch = false;
		else
			ProgramPtr++;
	}
}

void Machine::InitCommandMap()
{
	Cmd["NOP"] = &Machine::C_Nop;
	Cmd["PUSH"] = &Machine::C_Push;
}

void Machine::SetError(std::string error)
{
	Error = String::Format("%i: %s", Line->SourceLineNumber, error.c_str());
	Running = false;
	Util::Error(Error);
}

void Machine::Push(CommandParam* param)
{
	Stack.push(param);
}

CommandParam* Machine::Pop()
{
	if (Stack.empty()) {
		SetError(ERR_STACK_EMPTY);
		return NULL;
	}

	CommandParam* param = Stack.top();
	Stack.pop();
	return param;
}

void Machine::C_Nop()
{
	if (Line->HasParams()) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	// No operation
}

void Machine::C_Push()
{
	if (!Line->HasParams()) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	for (CommandParam* param : Line->GetParams())
		Push(param);
}
