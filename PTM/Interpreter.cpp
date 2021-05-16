#include "Interpreter.h"
#include "Util.h"

#define CYCLE_DELAY 1

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
	Env->Cycles = 0;
	Program = program;
	ProgramPtr = 0;
	Running = true;
	Paused = false;

	SDL_Event e;

	while (Running) {
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			Running = false;
			return;
		}
		if (Paused) {
			continue;
		}
		if (e.type == SDL_KEYDOWN) {
			// handle key events here
		}
		int oldPtr = ProgramPtr;
		ProgramLine* line = program->GetLine(ProgramPtr);
		CurrentLine = line;
		Execute(line);
		Env->Cycles++;
		if (!Running) {
			break;
		}
		if (ProgramPtr == oldPtr) {
			ProgramPtr++;
			if (ProgramPtr >= program->GetSize()) {
				FatalError("Execution pointer past end of program");
			}
		}
	}
}

void Interpreter::FatalError(std::string msg)
{
	ShowErrorMessageBox(msg);
	Running = false;
}

void Interpreter::ErrorOutOfParams()
{
	FatalError(String::Format("Out of parameters in line %i", 
		CurrentLine->GetSourceLineNumber()));
}

std::string Interpreter::NextString()
{
	if (ParamPtr >= CurrentLine->GetParamCount()) {
		ErrorOutOfParams();
		return "";
	}

	Parameter* param = CurrentLine->GetParam(ParamPtr);
	ParamPtr++;
	if (param->GetType() == ParameterType::PARAM_STRINGLITERAL ||
		param->GetType() == ParameterType::PARAM_LABEL)
		return param->GetStringValue();
	else if (param->GetType() == ParameterType::PARAM_VARIABLE)
		return Env->GetStringVar(param->GetStringValue());

	return "";
}

int Interpreter::NextNumber()
{
	if (ParamPtr >= CurrentLine->GetParamCount()) {
		ErrorOutOfParams();
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

std::string Interpreter::NextVariableIdentifier()
{
	if (ParamPtr >= CurrentLine->GetParamCount()) {
		ErrorOutOfParams();
		return "";
	}

	std::string ident = CurrentLine->GetParam(ParamPtr)->GetStringValue();
	ParamPtr++;
	return ident;
}

void Interpreter::Execute(ProgramLine* line)
{
	std::string& command = line->GetCommand();
	ParamPtr = 0;

	if (command == "EXIT") {
		Running = false;
	}
	else if (command == "GOTO") {
		std::string label = NextString();
		ProgramPtr = Program->GetLabel(label);
	}
	else if (command == "CALL") {
		std::string label = NextString();
		Env->PushToCallStack(ProgramPtr + 1);
		ProgramPtr = Program->GetLabel(label);
	}
	else if (command == "RETURN") {
		ProgramPtr = Env->PopFromCallStack();
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
		std::string var = NextVariableIdentifier();
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
	else if (command == "CYCLE") {
		std::string var = NextVariableIdentifier();
		Env->SetVar(var, Env->Cycles);
	}
	else if (command == "RANDOM") {
		std::string var = NextVariableIdentifier();
		int max = NextNumber();
		Env->SetVar(var, Util::Random(max));
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
		FatalError(String::Format("Invalid command in line %i: %s", 
			line->GetSourceLineNumber(), command.c_str()));
	}
}
