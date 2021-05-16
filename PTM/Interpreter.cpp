#include "Interpreter.h"
#include "Util.h"

Interpreter::Interpreter() : 
	Env(NULL), Program(NULL), CurrentLine(NULL), ProgramPtr(0), 
	Running(false), Paused(false), ParamPtr(0)
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

std::string Interpreter::NextString()
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

int Interpreter::NextNumber()
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
			std::string msg = NextString();
			ShowMessageBox("", msg);
		}
		else if (line->GetParamCount() == 2) {
			std::string title = NextString();
			std::string msg = NextString();
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
	else if (command == "SCREEN") {
		int cols = NextNumber();
		int rows = NextNumber();
		int width = NextNumber();
		int height = NextNumber();
		int fullscreen = NextNumber();

		Env->Gr = new Graphics(8 * cols, 8 * rows, width, height, fullscreen);
		Env->Ui = new UIContext(Env->Gr, 0xffffff, 0x000000);
		Env->Ui->Clear();
	}
	else if (command == "CLS") {
		Env->Ui->Clear();
	}
	else if (command == "LOCATE") {
		int x = NextNumber();
		int y = NextNumber();
		Env->GfxCursorX = x;
		Env->GfxCursorY = y;
	}
	else if (command == "FORECOLOR") {
		int fgc = NextNumber();
		Env->Ui->ForeColor = fgc;
	}
	else if (command == "BACKCOLOR") {
		int bgc = NextNumber();
		Env->Ui->BackColor = bgc;
	}
	else if (command == "COLOR") {
		int fgc = NextNumber();
		int bgc = NextNumber();
		Env->Ui->ForeColor = fgc;
		Env->Ui->BackColor = bgc;
	}
	else if (command == "PRINT") {
		std::string val = NextString();
		Env->Ui->Print(Env->GfxCursorX, Env->GfxCursorY, val);
	}
	else if (command == "REFRESH") {
		Env->Ui->Update();
	}
	else {
		FatalError(String::Format("Invalid command at line %i: %s", 
			line->GetSourceLineNumber(), command.c_str()));
	}
}
