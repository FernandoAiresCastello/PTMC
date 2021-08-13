#include "Machine.h"

#define CMD(cmd,fn) CmdMap[cmd] = &Machine::fn

Machine::Machine()
{
	Running = false;
	Halted = false;
	CurrentLine = nullptr;
	Win = nullptr;
	Pal = TPalette::Default;
	Chars = TCharset::Default;
	BackColor = 0;
	Board = nullptr;
	View = nullptr;

	InitCommandMap();
}

Machine::~Machine()
{
	for (int i = 0; i < Program.size(); i++) {
		delete Program[i];
		Program[i] = nullptr;
	}
	Program.clear();

	delete Win;
}

void Machine::LoadProgram(std::string filename)
{
	Program.clear();
	Labels.clear();

	int sourceLineNumber = 1;
	int actualLineNumber = 0;

	for (auto& srcline : TFile::ReadLines(filename)) {
		std::string line = TString::Trim(srcline);
		if (!line.empty() && !TString::StartsWith(line, "//")) {
			if (TString::EndsWith(line, ':')) {
				Labels[TString::RemoveLast(line)] = actualLineNumber;
			}
			else {
				Program.push_back(new ProgramLine(sourceLineNumber, actualLineNumber, srcline));
				actualLineNumber++;
			}
		}
		sourceLineNumber++;
	}
}

void Machine::Run()
{
	ProgramPtr = 0;
	Running = true;

	while (Running) {
		if (View) {
			View->Draw();
			Win->Update();
		}

		SDL_Event e = { 0 };
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			Running = false;
			break;
		}
		else if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_RETURN && TKey::Alt() && Win) {
				Win->ToggleFullscreen();
				Win->Update();
			}
			else if (e.key.keysym.sym == 1073742048 /* BREAK key */) {
				Running = false;
				break;
			}
		}

		if (Halted) {
			continue;
		}

		CurrentLine = Program[ProgramPtr];
		auto fn = CmdMap[CurrentLine->Command];
		if (fn)
			(this->*fn)();
		else
			Abort("Unknown command");

		if (Running && !Halted) {
			ProgramPtr++;
			if (ProgramPtr >= Program.size()) {
				Abort("Program pointer past end of program", false);
			}
		}
	}
}

void Machine::Abort(std::string msg, bool showSource)
{
	Running = false;
	
	if (showSource) {
		msg = TString::Format("%s\n\n%i %s",
			msg.c_str(), CurrentLine->SourceLineNumber, CurrentLine->Source.c_str());
	}

	MessageBox(Win ? (HWND)Win->GetHandle() : NULL, 
		msg.c_str(), "Runtime error", MB_ICONERROR | MB_OK);
}

Parameter Machine::Pop()
{
	if (ParamStack.empty()) {
		Abort("Stack empty");
		return Parameter();
	}
	else {
		Parameter param = ParamStack.top();
		ParamStack.pop();
		return param;
	}
}

int Machine::PopNumber()
{
	return Pop().Number;
}

std::string Machine::PopString()
{
	return Pop().String;
}

void Machine::InitCommandMap()
{
	CMD("NOP", C_Nop);
	CMD("EXIT", C_Exit);
	CMD("HALT", C_Halt);
	CMD("PUSH", C_Push);
	CMD("WINDOW.OPEN", C_WindowOpen);
	CMD("WINDOW.CLEAR", C_WindowClear);
	CMD("WINDOW.UPDATE", C_WindowUpdate);
}

void Machine::C_Nop()
{
	// No operation
}

void Machine::C_Exit()
{
	Running = false;
}

void Machine::C_Halt()
{
	Halted = true;
}

void Machine::C_Push()
{
	for (int i = 0; i < CurrentLine->GetParamCount(); i++)
		ParamStack.push(Parameter(*CurrentLine->NextParam()));
}

void Machine::C_WindowOpen()
{
	if (Win) {
		Abort("Window already open");
		return;
	}

	int fullscreen = PopNumber();
	int zoom = PopNumber();
	int h = PopNumber();
	int w = PopNumber();

	Win = new TWindow(w, h, zoom, fullscreen);
	Board = new TBoard(Win->Cols, Win->Rows, 2);
	View = new TBoardView(Board, Win, Chars, Pal, 0, 0, Win->Cols, Win->Rows, 255);

	Win->Clear(Pal, BackColor);
}

void Machine::C_WindowClear()
{
	BackColor = PopNumber();
	Win->Clear(Pal, BackColor);
}

void Machine::C_WindowUpdate()
{
	Win->Update();
}
