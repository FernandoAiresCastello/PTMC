#include "Interpreter.h"
#include "Util.h"

Interpreter::Interpreter() : 
	Env(NULL), Program(NULL), CurrentLine(NULL), ProgramPtr(0), Running(false), Paused(false), ParamPtr(0)
{
}

Interpreter::~Interpreter()
{
}

void Interpreter::Run(class Program* program, Environment* env)
{
	Env = env;
	Program = program;
	ProgramPtr = 0;
	Running = true;
	Paused = false;

	while (Running) {
		if (Paused) {
			continue;
		}
		int oldPtr = ProgramPtr;
		ProgramLine* line = program->GetLine(ProgramPtr);
		CurrentLine = line;
		Execute(line);
		if (!Running) {
			break;
		}
		if (ProgramPtr == oldPtr) {
			ProgramPtr++;
			if (ProgramPtr >= program->GetSize()) {
				Running = false;
				ShowErrorMessageBox("Execution pointer past end of program");
			}
		}
	}
}

void Interpreter::FatalError(std::string msg)
{
	ShowErrorMessageBox(msg);
	Running = false;
}

std::string Interpreter::NextStringParam()
{
	if (ParamPtr >= CurrentLine->GetParamCount()) {
		FatalError("Insufficient parameters");
		return "";
	}

	Parameter* param = CurrentLine->GetParam(ParamPtr);
	ParamPtr++;
	if (param->GetType() == ParameterType::PARAM_STRINGLITERAL)
		return param->GetStringValue();
	else if (param->GetType() == ParameterType::PARAM_VARIABLE)
		return Env->GetStringVar(param->GetStringValue());

	return "";
}

int Interpreter::NextNumericParam()
{
	if (ParamPtr >= CurrentLine->GetParamCount()) {
		FatalError("Insufficient parameters");
		return 0;
	}

	Parameter* param = CurrentLine->GetParam(ParamPtr);
	ParamPtr++;
	if (param->GetType() == ParameterType::PARAM_NUMERIC)
		return param->GetNumericValue();
	else if (param->GetType() == ParameterType::PARAM_VARIABLE)
		return Env->GetNumericVar(param->GetStringValue());

	return 0;
}

void Interpreter::Execute(ProgramLine* line)
{
	std::string& command = line->GetCommand();
	ParamPtr = 0;

	if (command == "EXIT") {
		Running = false;
	}
	else if (command == "MSGBOX") {
		if (line->GetParamCount() == 1) {
			std::string msg = NextStringParam();
			ShowMessageBox("", msg);
		}
		else if (line->GetParamCount() == 2) {
			std::string title = NextStringParam();
			std::string msg = NextStringParam();
			ShowMessageBox(title, msg);
		}
	}
	else if (command == "VAR") {
		std::string var = line->GetParam(0)->GetStringValue();
		Parameter* param2 = line->GetParam(1);
		
		if (param2->GetType() == ParameterType::PARAM_VARIABLE) {
			std::string value = Env->GetStringVar(param2->GetStringValue());
			Env->SetVar(var, value);
		}
		else {
			std::string value = param2->GetStringValue();
			Env->SetVar(var, value);
		}
	}
	else if (command == "CREATE_WINDOW") {
		int cols = NextNumericParam();
		int rows = NextNumericParam();
		int width = NextNumericParam();
		int height = NextNumericParam();
		int fullscreen = NextNumericParam();

		Env->Gr = new Graphics(8 * cols, 8 * rows, width, height, fullscreen);
	}
	else {
		FatalError(String::Format("Invalid command at line %i: %s", 
			line->GetSourceLineNumber(), command.c_str()));
	}
}
