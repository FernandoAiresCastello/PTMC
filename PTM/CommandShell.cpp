#include <vector>
#include <string>
#include <algorithm>
#include "CommandShell.h"
#include "SharedScreen.h"
#include "SharedObjects.h"
#include "ProgramEditor.h"
#include "Program.h"

struct {
	bool Running;
} Shell;

struct ShellCommand {
	std::string Name;
	std::vector<std::string> Args;
	bool Empty;
};

struct {
	const std::string NotImplemented = "Not implemented";
	const std::string SyntaxError = "Syntax error";
	const std::string WrongNumArgs = "Wrong arguments";
} ShellError;

void InitCommandShell()
{
}

void DestroyCommandShell()
{
}

void RunCommandShell()
{
	Shell.Running = true;

	PrintIntro();

	while (Shell.Running) {
		DrawScreenBorder();
		DrawScreenBuffer();
		UpdateCursor();
		UpdateEntireScreen();

		SDL_Event e = { 0 };
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			Shell.Running = false;
			break;
		}
		else if (e.type == SDL_KEYDOWN) {
			SDL_Keycode key = e.key.keysym.sym;
			bool alt = Key::Alt();
			bool ctrl = Key::Ctrl();
			bool shift = Key::Shift();

			if (key == SDLK_RETURN && alt) {
				ToggleFullscreen();
			}
			else if (key == SDLK_RIGHT) {
				CursorMove(1, 0);
			}
			else if (key == SDLK_LEFT) {
				CursorMove(-1, 0);
			}
			else if (key == SDLK_UP) {
				CursorMove(0, -1);
			}
			else if (key == SDLK_DOWN) {
				CursorMove(0, 1);
			}
			else if (key == SDLK_HOME) {
				if (ctrl) {
					CursorHome();
				}
				else if (shift) {
					ClearScreen();
				}
				else {
					CursorMoveToStartOfLine();
				}
			}
			else if (key == SDLK_END) {
				if (ctrl) {
					CursorEnd();
				}
				else {
					CursorMoveToEndOfLine();
				}
			}
			else if (key == SDLK_BACKSPACE) {
				BackSpace();
			}
			else if (key == SDLK_DELETE) {
				DeleteCharUnderCursor();
			}
			else if (key == SDLK_RETURN) {
				InterpretCurrentLine();
			}
			else if (key == SDLK_INSERT) {
				ToggleInsertMode();
			}
			else if (IsTypableChar(key)) {
				int ch = (int)key;
				if (Key::CapsLock() || shift) {
					PutChar(shift ? String::ShiftChar(toupper(ch)) : toupper(ch));
				}
				else {
					PutChar(shift ? String::ShiftChar(ch) : ch);
				}
			}
		}
	}
}

void Debug()
{
}

void PrintIntro()
{
	PrintLine(APPLICATION_NAME);
	PrintLine("Ok");
}

ShellCommand ParseCommandLine(std::string commandLine)
{
	ShellCommand cmd;
	std::string line = String::Trim(commandLine);
	cmd.Empty = line.empty();
	
	if (!cmd.Empty) {
		std::string name;
		int i = 0;
		while (true) {
			char ch = line[i];
			if (ch == ' ' || i >= line.length())
				break;

			name += ch;
			i++;
		}
		cmd.Name = String::ToLower(name);

		std::string arglist = line.substr(name.length());
		cmd.Args = String::Split(arglist, ',');
	}

	return cmd;
}

void InterpretCurrentLine()
{
	const ShellCommand cmdobj = ParseCommandLine(GetCurrentLineTrimmed());
	Crlf();
	if (cmdobj.Empty)
		return;

	const std::string cmd = cmdobj.Name;
	const auto args = cmdobj.Args;
	const int argc = args.size();

	std::string error = "";
	bool syntaxError = false;

	if (cmd == "debug") {
		Debug();
	}
	else if (cmd == "exit") {
		Shell.Running = false;
	}
	else if (cmd == "cls") {
		ClearScreen();
	}
	else if (cmd == "color") {
		if (argc == 1) {
			int fg = String::ToInt(args[0]);
			SetForeColor(fg);
		}
		else if (argc == 2) {
			int fg = String::ToInt(args[0]);
			int bg = String::ToInt(args[1]);
			SetForeColor(fg);
			SetBackColor(bg);
		}
		else if (argc == 3) {
			int fg = String::ToInt(args[0]);
			int bg = String::ToInt(args[1]);
			int border = String::ToInt(args[2]);
			SetColors(fg, bg, border);
		}
		else {
			error = ShellError.WrongNumArgs;
		}
	}
	else if (cmd == "fgcolor") {
		if (argc == 1) {
			int fg = String::ToInt(args[0]);
			SetForeColor(fg);
		}
		else {
			error = ShellError.WrongNumArgs;
		}
	}
	else if (cmd == "bgcolor") {
		if (argc == 1) {
			int bg = String::ToInt(args[0]);
			SetBackColor(bg);
		}
		else {
			error = ShellError.WrongNumArgs;
		}
	}
	else if (cmd == "bdcolor") {
		if (argc == 1) {
			int border = String::ToInt(args[0]);
			SetBorderColor(border);
		}
		else {
			error = ShellError.WrongNumArgs;
		}
	}
	else if (cmd == "chdir") {
		if (argc == 1) {
			SetWorkingDir(args[0]);
		}
		else {
			error = ShellError.WrongNumArgs;
		}
	}
	else if (cmd == "files") {
		if (argc == 0) {
			PrintFiles("");
		}
		else if (argc == 1) {
			PrintFiles(args[0]);
		}
		else {
			error = ShellError.WrongNumArgs;
		}
	}
	else if (cmd == "prog") {
		RunProgramEditor();
		ClearScreen();
	}
	else if (cmd == "saveprog") {
		if (argc == 1) {
			SaveProgram(args[0]);
		}
		else {
			error = ShellError.WrongNumArgs;
		}
	}
	else if (cmd == "loadprog") {
		if (argc == 1) {
			LoadProgram(args[0]);
		}
		else {
			error = ShellError.WrongNumArgs;
		}
	}
	else {
		syntaxError = true;
	}

	if (syntaxError) {
		PrintLine(ShellError.SyntaxError);
	}
	else if (!error.empty()) {
		PrintLine(error);
	}

	PrintLine("Ok");
}

void SetWorkingDir(std::string dir)
{
	if (File::Exists(dir)) {
		SetSharedDirectory(dir);
	}
	else {
		PrintLine("Path not found");
	}
}

void PrintFiles(std::string pattern)
{
	auto files = File::List(GetSharedDirectory(), "*" + pattern, true);
	for (int i = 0; i < files.size(); i++) {
		std::string& file = files[i];
		if (file != "." && file != ".." && !File::IsDirectory(file)) {
			PrintLine(file);
			DrawScreenBuffer();
			UpdateEntireScreen();
		}
	}
}

void SaveProgram(std::string filename)
{
	std::string fileContents = "";
	Program* prog = GetProgram();
	for (int i = 0; i < prog->Lines.size(); i++) {
		fileContents.append(prog->Lines[i]->Command);
		fileContents.append("\n");
	}
	File::WriteText(filename, fileContents);
}

void LoadProgram(std::string filename)
{
	auto lines = File::ReadLines(filename);
	SetProgramLines(lines);
}
