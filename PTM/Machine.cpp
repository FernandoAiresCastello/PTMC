#include "Machine.h"

#define CMD(cmd,impl) Cmd[cmd] = &Machine::impl;

#define ERR_END_OF_PROGRAM "End of program"
#define ERR_INVALID_COMMAND "Invalid command"
#define ERR_SYNTAX_ERROR "Syntax error"
#define ERR_STACK_EMPTY "Stack empty"
#define ERR_CALL_STACK_EMPTY "Call stack empty"
#define ERR_LABEL_NOT_FOUND "Label not found"

Machine::Machine()
{
	Running = false;
	ProgramPtr = 0;
	Branch = false;
	Error = "";
	DefaultChars = new Charset();
	DefaultChars->InitDefaultCharset();
	InitCommandMap();
}

Machine::~Machine()
{
	delete DefaultChars;
	delete Debugger;
}

void Machine::Run(Datafile* data, Graphics* gr)
{
	Running = true;
	Prog = data->GetProgram();
	Gr = gr;
	Debugger = new class Debugger(data, gr);
	
	while (Running) {
		
		Error = "";
		ProcessEvents();

		if (!Running)
			break;

		if (ProgramPtr >= Prog->GetSize()) {
			Line = NULL;
			Abort(ERR_END_OF_PROGRAM);
			break;
		}

		Line = Prog->GetLine(ProgramPtr);
		Line->ResetParamIndex();

		auto& impl = Cmd[Line->Command];
		if (impl != NULL) {
			((*this).*impl)();
		}
		else {
			Abort(ERR_INVALID_COMMAND);
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

void Machine::ProcessEvents()
{
	SDL_Event e = { 0 };
	SDL_PollEvent(&e);

	if (e.type == SDL_QUIT)
		Running = false;
	else if (e.type == SDL_KEYDOWN)
		OnKeyPress(e.key.keysym.sym);
}

void Machine::OnKeyPress(SDL_Keycode key)
{
	bool shift = Key::Shift();
	bool ctrl = Key::Ctrl();
	bool alt = Key::Alt();

	if (alt) {
		if (key == SDLK_RETURN) {
			Gr->ToggleFullscreen();
			Gr->Update();
		}
	}
}

void Machine::Abort(std::string error)
{
	const int fgc = 0xffffff;
	const int bgc = 0xff0000;

	ClearScreen(0xff0000);
	Print(error, 1, 1, 0xffffff, 0xff0000);
	
	if (Line != NULL) {
		Print(String::Format("At line %i:", Line->SourceLineNumber), 1, 3, fgc, bgc);
		Print(Line->SourceLine, 1, 5, fgc, bgc);
	}

	UpdateScreen();
	Halt();
}

void Machine::Halt()
{
	while (Running) {
		ProcessEvents();
	}
}

void Machine::Goto(std::string label)
{
	int target = Prog->GetLabelTarget(label);
	if (target < 0) {
		Abort(ERR_LABEL_NOT_FOUND);
		return;
	}

	ProgramPtr = target;
	Branch = true;
}

void Machine::Call(std::string label)
{
	int target = Prog->GetLabelTarget(label);
	if (target < 0) {
		Abort(ERR_LABEL_NOT_FOUND);
		return;
	}

	CallStack.push(ProgramPtr);
	ProgramPtr = target;
	Branch = true;
}

void Machine::Return()
{
	if (CallStack.empty()) {
		Abort(ERR_CALL_STACK_EMPTY);
		return;
	}

	ProgramPtr = CallStack.top() + 1;
	CallStack.pop();
	Branch = true;
}

void Machine::Print(std::string text, int x, int y, int fgc, int bgc)
{
	Gr->Print(DefaultChars, x, y, fgc, bgc, text);
}

void Machine::ClearScreen(int bgc)
{
	Gr->Clear(bgc);
}

void Machine::UpdateScreen()
{
	Gr->Update();
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
		Abort(ERR_STACK_EMPTY);
		return "";
	}

	StackElement element = Stack.top();
	Stack.pop();
	return element.StringValue;
}

int Machine::PopNumber()
{
	if (Stack.empty()) {
		Abort(ERR_STACK_EMPTY);
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
	CMD("HALT", C_Halt);
	CMD("DEBUG", C_Debug);
	CMD("CALL", C_Call);
	CMD("GOTO", C_Goto);
	CMD("RET", C_Return);
}

void Machine::C_Goto()
{
	if (!Line->HasParams(1)) {
		Abort(ERR_SYNTAX_ERROR);
		return;
	}
	if (Line->GetParam(0)->Type != CommandParamType::Label) {
		Abort(ERR_SYNTAX_ERROR);
		return;
	}

	Goto(Line->GetParam(0)->String);
}

void Machine::C_Call()
{
	if (!Line->HasParams(1)) {
		Abort(ERR_SYNTAX_ERROR);
		return;
	}
	if (Line->GetParam(0)->Type != CommandParamType::Label) {
		Abort(ERR_SYNTAX_ERROR);
		return;
	}

	Call(Line->GetParam(0)->String);
}

void Machine::C_Return()
{
	if (Line->HasParams()) {
		Abort(ERR_SYNTAX_ERROR);
		return;
	}

	Return();
}

void Machine::C_Debug()
{
	if (Line->HasParams()) {
		Abort(ERR_SYNTAX_ERROR);
		return;
	}

	Debugger->Run();
}

void Machine::C_Halt()
{
	if (Line->HasParams()) {
		Abort(ERR_SYNTAX_ERROR);
		return;
	}

	Halt();
}

void Machine::C_Var()
{
	if (!Line->HasParams(2)) {
		Abort(ERR_SYNTAX_ERROR);
		return;
	}
	if (Line->GetParam(0)->Type != CommandParamType::Variable) {
		Abort(ERR_SYNTAX_ERROR);
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
		Abort(ERR_SYNTAX_ERROR);
		return;
	}

	Running = false;
}

void Machine::C_Push()
{
	if (!Line->HasParams()) {
		Abort(ERR_SYNTAX_ERROR);
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
			Abort(ERR_SYNTAX_ERROR);
			return;
		}
	}
}

void Machine::C_Nop()
{
	if (Line->HasParams()) {
		Abort(ERR_SYNTAX_ERROR);
		return;
	}
}

void Machine::C_Brk()
{
	if (Line->HasParams()) {
		Abort(ERR_SYNTAX_ERROR);
		return;
	}

	__debugbreak();
}
