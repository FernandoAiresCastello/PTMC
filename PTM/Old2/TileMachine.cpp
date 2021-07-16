#include <map>
#include <stack>
#include <vector>
#include "TileMachine.h"
#include "SharedScreen.h"
#include "SharedObjects.h"
#include "GameData.h"
#include "Commands.h"
#include <TBRLGPT.h>
using namespace TBRLGPT;

struct Machine* Machine = NULL;

void InitTileMachine()
{
	Machine = new struct Machine();
	Machine->Gr = GetScreenGraphics();
	Machine->GameData = new GameData();
	Machine->GameData->SetCharset(GetSharedCharset());
	Machine->GameData->SetPalette(GetSharedPalette());
	InitCommandTable();
}

void DestroyTileMachine()
{
	delete Machine->GameData;
}

void RestartTileMachine()
{
	Machine->Program.clear();
	Machine->ProgramLabels.clear();
	Machine->Defs.clear();
	Machine->ProgramPtr = 0;
	Machine->Error = "";
	Machine->Running = true;
	Machine->Halted = false;
	while (!Machine->Stack.empty()) {
		Machine->Stack.pop();
	}
	ClearMachineScreen();
	RefreshMachineScreen();
}

std::string GetTileMachineError()
{
	return Machine->Error;
}

void ClearMachineScreen()
{
	Machine->Gr->Clear(Machine->BackColor);
}

void RefreshMachineScreen()
{
	Machine->Gr->Update();
}

void SetError(std::string error)
{
	Machine->Error = String::Format("%i:%s", Machine->Line->SourceLineNumber, error.c_str());
}

void BranchTo(std::string label)
{
	if (HasLabel(label)) {
		Machine->Branching = true;
		Machine->ProgramPtr = Machine->ProgramLabels[label];
	}
	else {
		SetError(ERR_LABEL_NOT_FOUND);
	}
}

bool HasLabel(std::string label)
{
	return Machine->ProgramLabels.find(label) != Machine->ProgramLabels.end();
}

bool HasDef(std::string def)
{
	return Machine->Defs.find(def) != Machine->Defs.end();
}

void CompileAndRunProgram(Program* prog)
{
	RestartTileMachine();
	Machine->GameData->SetProgram(prog);

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
			Machine->ProgramLabels[label] = actualLineIndex;
			continue;
		}
		// collect then skip defs
		if (String::StartsWith(command, ".def")) {
			Machine->Defs[parts[1]] = parts[2];
			continue;
		}

		command = String::ToUpper(command);
		int paramNumber = 0;
		std::string paramString = "";
		bool hasParam = false;

		if (parts.size() > 1) {
			hasParam = true;
			std::string param = parts[1];
			if (HasDef(param)) {
				param = Machine->Defs[param];
			}
			paramString = param;
			paramNumber = String::ToInt(param);
		}

		CompiledProgramLine cl;
		cl.SourceLineNumber = i + 1;
		cl.ActualLineIndex = actualLineIndex;
		cl.Command = command;
		cl.HasParam = hasParam;
		cl.ParamNumber = paramNumber;
		cl.ParamString = paramString;

		Machine->Program.push_back(cl);

		actualLineIndex++;
	}

	ExecuteCompiledProgram();
}

void ExecuteCompiledProgram()
{
	if (Machine->Program.empty()) {
		Machine->Error = ERR_PROGRAM_EMPTY;
		return;
	}

	while (Machine->Running) {

		SDL_Event e = { 0 };
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			Machine->Running = false;
			continue;
		}
		else if (e.type == SDL_KEYDOWN) {
			SDL_Keycode key = e.key.keysym.sym;
			if (key == SDLK_ESCAPE) {
				Machine->Running = false;
				continue;
			}
		}

		if (Machine->Halted)
			continue;

		Machine->Branching = false;

		Machine->Line = &Machine->Program[Machine->ProgramPtr];
		std::string& cmd = Machine->Program[Machine->ProgramPtr].Command;
		auto impl = Cmd[cmd];
		if (impl) {
			impl();
		}
		else {
			SetError("Invalid command");
		}

		if (!Machine->Error.empty()) {
			Machine->Running = false;
			continue;
		}
		if (Machine->Running && !Machine->Halted && !Machine->Branching) {
			Machine->ProgramPtr++;
		}
		if (Machine->ProgramPtr >= Machine->Program.size()) {
			Machine->Running = false;
			Machine->Error = String::Format("%i:" ERR_END_OF_PROGRAM, 
				Machine->Program[Machine->Program.size() - 1].SourceLineNumber + 1);
		}
	}
}
