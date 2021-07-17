#include "Machine.h"

#define CMD(cmd,impl) Cmd[cmd] = &Machine::impl;

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

void Machine::Run(Datafile* data, Graphics* gr)
{
	Program* prog = data->GetProgram();
	Gr = gr;

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
		if (!Running)
			break;
		if (Branch)
			Branch = false;
		else
			ProgramPtr++;
	}
}

void Machine::SetError(std::string error)
{
	Error = String::Format("%i: %s", Line->SourceLineNumber, error.c_str());
	Running = false;
	Util::Error(Error);
}

void Machine::PushString(std::string value)
{
	StackElement element;
	element.StringValue = value;
	element.NumberValue = String::ToInt(value);
	Stack.push(element);
}

void Machine::PushNumber(int value)
{
	StackElement element;
	element.StringValue = String::ToString(value);
	element.NumberValue = value;
	Stack.push(element);
}

std::string Machine::PopString()
{
	if (Stack.empty()) {
		SetError(ERR_STACK_EMPTY);
		return "";
	}

	StackElement element = Stack.top();
	Stack.pop();
	return element.StringValue;
}

int Machine::PopNumber()
{
	if (Stack.empty()) {
		SetError(ERR_STACK_EMPTY);
		return 0;
	}

	StackElement element = Stack.top();
	Stack.pop();
	return element.NumberValue;
}

void Machine::SetVar(std::string name, std::string value)
{
	Variable var;
	var.Name = name;
	var.StringValue = value;
	var.NumberValue = String::ToInt(value);
	Vars[name] = var;
}

void Machine::SetVar(std::string name, int value)
{
	Variable var;
	var.Name = name;
	var.StringValue = String::ToString(value);
	var.NumberValue = value;
	Vars[name] = var;
}

std::string Machine::GetVarAsString(std::string name)
{
	return Vars[name].StringValue;
}

int Machine::GetVarAsNumber(std::string name)
{
	return Vars[name].NumberValue;
}

void Machine::InitCommandMap()
{
	CMD("NOP", C_Nop);
	CMD("BRK", C_Brk);
	CMD("PUSH", C_Push);
	CMD("EXIT", C_Exit);
	CMD("VAR", C_Var);
}

void Machine::C_Var()
{
	if (!Line->HasParams(2)) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}
	if (Line->GetParam(0)->Type != CommandParamType::Variable) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	std::string name = Line->NextParam()->String;
	CommandParam* valueParam = Line->NextParam();
	if (valueParam->Type == CommandParamType::NumberLiteral)
		SetVar(name, valueParam->Number);
	else if (valueParam->Type == CommandParamType::StringLiteral)
		SetVar(name, valueParam->String);
	if (valueParam->Type == CommandParamType::Variable)
		SetVar(name, GetVarAsString(valueParam->String));
}

void Machine::C_Exit()
{
	if (Line->HasParams()) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	Running = false;
}

void Machine::C_Push()
{
	if (!Line->HasParams()) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	for (CommandParam* param : Line->GetParams()) {
		if (param->Type == CommandParamType::NumberLiteral)
			PushNumber(param->Number);
		else if (param->Type == CommandParamType::StringLiteral)
			PushString(param->String);
		else if (param->Type == CommandParamType::Variable)
			PushString(GetVarAsString(param->String));
		else {
			SetError(ERR_SYNTAX_ERROR);
			return;
		}
	}
}

void Machine::C_Nop()
{
	if (Line->HasParams()) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}
}

void Machine::C_Brk()
{
	if (Line->HasParams()) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	__debugbreak();
}
