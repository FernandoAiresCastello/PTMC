#include "Machine.h"

#define CMD(cmd,fn) CmdMap[cmd] = &Machine::fn

Machine::Machine()
{
	Running = false;
	CurrentLine = nullptr;
	Win = nullptr;

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
		if (!line.empty()) {
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
		CurrentLine = Program[ProgramPtr];
		auto fn = CmdMap[CurrentLine->Command];
		if (fn)
			(this->*fn)();
		else
			Abort("Unknown command");

		if (Running) {
			ProgramPtr++;
			if (ProgramPtr >= Program.size()) {
				Running = false;
			}
		}
	}
}

void Machine::Abort(std::string msg)
{
	Running = false;
	msg = TString::Format("%s\n\n%i %s", 
		msg.c_str(), CurrentLine->SourceLineNumber, CurrentLine->Source.c_str());
	MessageBox(Win ? (HWND)Win->GetHandle() : NULL, 
		msg.c_str(), "Runtime error", MB_ICONERROR | MB_OK);
}

void Machine::InitCommandMap()
{
	CMD("NOP", C_Nop);
	CMD("PUSH", C_Push);
}

void Machine::C_Nop()
{
	// No operation
}

void Machine::C_Push()
{
	ParamStack.push(Parameter(*CurrentLine->GetParam()));
}
