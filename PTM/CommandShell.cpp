#include <vector>
#include <string>
#include <algorithm>
#include "CommandShell.h"

struct {
	Project* Proj;
	std::string WorkingDir;
	bool Running;
} Shell;

struct {
	Graphics* Gr;
	int Cols;
	int Rows;
	UIContext* Ctx;
	Charset* Chr;
	Palette* Pal;
	Map* Buf;
	MapViewport* View;

	struct {
		int Fg;
		int Bg;
		int Border;
	} Color;

	struct {
		int X;
		int Y;
		bool Visible;
		bool Insert;
	} Cursor;

} Screen;

struct ShellCommand {
	std::string Name;
	std::vector<std::string> Args;
	bool Empty;
};

struct {
	const int Null = 0;
	const int Space = 32;
	const int Insert = 127;
} SpecialChar;

struct {
	const int Minimum = 32;
	const int Maximum = 126;
} TypableChar;

struct {
	const int Fore = 1;
	const int Back = 52;
	const int Border = 50;
} DefaultColor;

struct {
	const std::string NotImplemented = "Command not implemented";
	const std::string SyntaxError = "Syntax error";
	const std::string WrongNumArgs = "Wrong number of arguments";
} Error;

struct ProgramLine {
	std::string Command;
};

struct {
	std::vector<ProgramLine*> Lines;
} Program;

void RunCommandShell()
{
	Shell.Running = true;
	Shell.Proj = new Project();
	Shell.Proj->Load("ptm.tgpro");
	Shell.WorkingDir = ".";

	InitScreen();
	InitProgramEditor();

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

			if (key == SDLK_ESCAPE) {
				Shell.Running = false;
				break;
			}
			else if (key == SDLK_F1) {
				Debug();
			}
			else if (key == SDLK_RETURN && alt) {
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

	ExitCommandShell();
}

void ExitCommandShell()
{
	delete Shell.Proj;
	Shell.Proj = NULL;
	Screen.Ctx->Chars = NULL;
	Screen.Ctx->Pal = NULL;
	delete Screen.Ctx;
	Screen.Ctx = NULL;
	delete Screen.Gr;
	Screen.Gr = NULL;

	for (int i = 0; i < Program.Lines.size(); i++) {
		ProgramLine* line = Program.Lines[i];
		delete line;
	}
	Program.Lines.clear();	
}

void InitScreen()
{
	Screen.Gr = new Graphics(256, 192, 800, 600, false);
	Screen.Gr->SetWindowTitle("PTM - Programmable Tile Machine");
	Screen.Cols = Screen.Gr->Cols;
	Screen.Rows = Screen.Gr->Rows;
	Screen.Ctx = new UIContext(Screen.Gr, 0xffffff, 0xff0000);
	Screen.Chr = Screen.Ctx->Chars;
	Screen.Pal = Screen.Ctx->Pal;
	Screen.Ctx->Clear();
	Screen.Chr->Clear();
	Screen.Ctx->Chars = Shell.Proj->GetCharset();
	Screen.Ctx->Pal = Shell.Proj->GetPalette();
	Screen.Buf = new Map(Shell.Proj, "main", Screen.Cols - 2, Screen.Rows - 2, 2);
	Screen.View = new MapViewport(
		Screen.Ctx, Screen.Buf, 1, 1, 
		Screen.Buf->GetWidth(), Screen.Buf->GetHeight(), 0, 0, 200);

	ShowCursor(true);
	Screen.Cursor.Insert = false;
	Locate(0, 0);
	SetColors(DefaultColor.Fore, DefaultColor.Back, DefaultColor.Border);
	ClearScreen();
	DrawScreenBuffer();
	DrawScreenBorder();
	UpdateEntireScreen();
}

void Debug()
{
	PrintLine("Hello Debug!");
}

void ToggleFullscreen()
{
	Screen.Gr->ToggleFullscreen();
}

void DrawScreenBuffer()
{
	Screen.View->Draw();
}

void DrawScreenBorder()
{
	const int color = Screen.Pal->Get(Screen.Color.Border)->ToInteger();
	for (int y = 0; y < Screen.Rows; y++) {
		Screen.Gr->PutChar(Screen.Chr, 0, 0, y, color, color);
		Screen.Gr->PutChar(Screen.Chr, 0, Screen.Cols - 1, y, color, color);
	}
	for (int x = 0; x < Screen.Cols; x++) {
		Screen.Gr->PutChar(Screen.Chr, 0, x, 0, color, color);
		Screen.Gr->PutChar(Screen.Chr, 0, x, Screen.Rows - 1, color, color);
	}
}

void UpdateEntireScreen()
{
	Screen.Ctx->Update();
}

void PrintIntro()
{
	PrintLine("Programmable Tile Machine");
	PrintLine("Interactive Shell");
	PrintLine("Version 0.1");
	PrintLine("Ok");
}

void ToggleInsertMode()
{
	Screen.Cursor.Insert = !Screen.Cursor.Insert;
}

void Locate(int x, int y)
{
	const int prevX = Screen.Cursor.X;
	const int prevY = Screen.Cursor.Y;

	Screen.Cursor.X = x;
	Screen.Cursor.Y = y;

	Screen.Buf->DeleteObject(prevX, prevY, 1);
}

void SetColors(int fg, int bg, int border)
{
	Screen.Color.Fg = fg;
	Screen.Color.Bg = bg;
	Screen.Color.Border = border;
	UpdateBufferColors();
}

void SetForeColor(int color) {
	Screen.Color.Fg = color;
	UpdateBufferColors();
}

void SetBackColor(int color) {
	Screen.Color.Bg = color;
	UpdateBufferColors();
}

void SetBorderColor(int color) {
	Screen.Color.Border = color;
	UpdateBufferColors();
}

void UpdateBufferColors() {
	for (int y = 0; y < Screen.Buf->GetHeight(); y++) {
		for (int x = 0; x < Screen.Buf->GetWidth(); x++) {
			Object* o = Screen.Buf->GetObject(x, y, 0);
			if (!o->IsVoid()) {
				ObjectChar& ch = o->GetAnimation().GetFrame(0);
				ch.ForeColorIx = Screen.Color.Fg;
				ch.BackColorIx = Screen.Color.Bg;
			}
		}
	}
}

void ClearScreen()
{
	for (int y = 0; y < Screen.Buf->GetHeight(); y++) {
		ClearLine(y);
	}
	CursorHome();
}

void ClearLine(int index)
{
	for (int x = 0; x < Screen.Buf->GetWidth(); x++) {
		Object* o = Screen.Buf->GetObject(x, index, 0);
		o->SetVoid(false);
		o->SetVisible(true);
		o->GetAnimation().Clear();
		o->GetAnimation().AddFrame(ObjectChar(0, Screen.Color.Fg, Screen.Color.Bg));
	}
}

std::string GetCurrentLine()
{
	return GetLine(Screen.Cursor.Y);
}

std::string GetCurrentLineTrimmed()
{
	std::string line;
	for (int x = 0; x < Screen.Buf->GetWidth(); x++) {
		Object* o = Screen.Buf->GetObject(x, Screen.Cursor.Y, 0);
		int chr = o->GetAnimation().GetFrame(0).Index;
		if (chr != SpecialChar.Null) {
			line.append(1, chr);
		}
	}
	return line;
}

std::string GetLine(int index)
{
	std::string line;
	for (int x = 0; x < Screen.Buf->GetWidth(); x++) {
		Object* o = Screen.Buf->GetObject(x, index, 0);
		line.append(1, o->GetAnimation().GetFrame(0).Index);
	}
	return line;
}

void Print(std::string text)
{
	Screen.Buf->SetStringOfObjects(
		text,
		Screen.Color.Fg, Screen.Color.Bg,
		Screen.Cursor.X, Screen.Cursor.Y, 0);

	int newX = Screen.Cursor.X += text.length();

	Locate(newX, Screen.Cursor.Y);
}

void Print(std::string text, int x, int y)
{
	Print(text, x, y, Screen.Color.Fg, Screen.Color.Bg);
}

void Print(std::string text, int x, int y, int fgc, int bgc)
{
	Screen.Buf->SetStringOfObjects(text, fgc, bgc, x, y, 0);
}

void PrintLine(std::string text)
{
	Print(text);
	Crlf();
}

void PrintOnBottomBorder(std::string text)
{
	Screen.Ctx->ForeColor = Screen.Pal->Get(Screen.Color.Fg)->ToInteger();
	Screen.Ctx->BackColor = Screen.Pal->Get(Screen.Color.Border)->ToInteger();
	Screen.Ctx->Print(Screen.Gr->Cols - text.length(), Screen.Gr->Rows - 1, text);
}

void PutChar(int ch)
{
	if (Screen.Cursor.Insert) {
		std::string line = GetCurrentLine();
		line.insert(line.begin() + Screen.Cursor.X, SpecialChar.Space);
		line = line.substr(0, Screen.Buf->GetWidth() - 1);
		Print(line, 0, Screen.Cursor.Y);
	}

	if (Screen.Cursor.X < Screen.Buf->GetWidth() - 1) {
		ObjectChar* under = GetCharUnderCursor();
		under->Index = ch;
		CursorMove(1, 0);
	}
}

void ShowCursor(bool show)
{
	Screen.Cursor.Visible = show;
}

void UpdateCursor()
{
	Screen.Buf->DeleteObject(Screen.Cursor.X, Screen.Cursor.Y, 1);

	Object* cursor = Screen.Buf->GetObject(Screen.Cursor.X, Screen.Cursor.Y, 1);

	if (Screen.Cursor.Visible) {
		if (cursor->IsVoid()) {
			cursor->SetVoid(false);
			cursor->SetVisible(true);
			ObjectChar* chr = GetCharUnderCursor();
			ObjectAnim& anim = cursor->GetAnimation();

			if (Screen.Cursor.Insert) {
				anim.AddFrame(ObjectChar(SpecialChar.Insert, Screen.Color.Fg, Screen.Color.Bg));
			}
			else {
				anim.AddFrame(ObjectChar(chr->Index, Screen.Color.Bg, Screen.Color.Fg));
			}

			if (chr->Index != SpecialChar.Null && chr->Index != SpecialChar.Space) {
				anim.AddFrame(ObjectChar(chr->Index, Screen.Color.Fg, Screen.Color.Bg));
			}
		}
	}
	else {
		cursor->SetVoid(true);
		cursor->SetVisible(true);
		cursor->GetAnimation().Clear();
	}
}

ObjectChar* GetCharUnderCursor()
{
	return GetCharAt(Screen.Cursor.X, Screen.Cursor.Y);
}

ObjectChar* GetCharAt(int x, int y)
{
	return &(Screen.Buf->GetObject(x, y, 0)->GetAnimation().GetFrame(0));
}

void Crlf()
{
	if (Screen.Cursor.Y < Screen.Buf->GetHeight() - 1) {
		Locate(0, Screen.Cursor.Y + 1);
	}
	else {
		ScrollBufferUp();
	}
}

void BackSpace()
{
	if (Screen.Cursor.X == 0)
		return;

	CursorMove(-1, 0);
	std::string line = GetCurrentLine();
	line.erase(line.begin() + Screen.Cursor.X);
	Print(line, 0, Screen.Cursor.Y);
}

void DeleteCharUnderCursor()
{
	std::string line = GetCurrentLine();
	line.erase(line.begin() + Screen.Cursor.X);
	Print(line, 0, Screen.Cursor.Y);
}

void CursorHome()
{
	Locate(0, 0);
}

void CursorEnd()
{
	Locate(Screen.Buf->GetWidth() - 1, Screen.Buf->GetHeight() - 1);
}

void CursorMove(int dx, int dy)
{
	int newX = Screen.Cursor.X + dx;
	int newY = Screen.Cursor.Y + dy;
	const int endX = Screen.Buf->GetWidth() - 1;

	if (newX < 0)
		newX = endX;
	else if (newX > endX)
		newX = 0;

	if (newY < 0 || newY >= Screen.Buf->GetHeight())
		return;

	Locate(newX, newY);
}

void CursorMoveToStartOfLine()
{
	Locate(0, Screen.Cursor.Y);
}

void CursorMoveToEndOfLine()
{
	int newX = 0;
	
	for (int x = Screen.Buf->GetWidth() - 1; x >= 0 ; x--) {
		ObjectChar* ch = GetCharAt(x, Screen.Cursor.Y);
		if (ch->Index != SpecialChar.Null) {
			newX = x + 1;
			break;
		}
	}

	Locate(newX, Screen.Cursor.Y);
}

void ScrollBufferUp()
{
	for (int y = 1; y < Screen.Buf->GetHeight(); y++) {
		std::string line = GetLine(y);
		Print(line, 0, y - 1);
	}
	ClearLine(Screen.Buf->GetHeight() - 1);
	Locate(0, Screen.Cursor.Y);
}

bool IsTypableChar(int ch)
{
	return ch >= TypableChar.Minimum && ch <= TypableChar.Maximum;
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
			error = Error.WrongNumArgs;
		}
	}
	else if (cmd == "fgcolor") {
		if (argc == 1) {
			int fg = String::ToInt(args[0]);
			SetForeColor(fg);
		}
		else {
			error = Error.WrongNumArgs;
		}
	}
	else if (cmd == "bgcolor") {
		if (argc == 1) {
			int bg = String::ToInt(args[0]);
			SetBackColor(bg);
		}
		else {
			error = Error.WrongNumArgs;
		}
	}
	else if (cmd == "bdcolor") {
		if (argc == 1) {
			int border = String::ToInt(args[0]);
			SetBorderColor(border);
		}
		else {
			error = Error.WrongNumArgs;
		}
	}
	else if (cmd == "chdir") {
		if (argc == 1) {
			SetWorkingDir(args[0]);
		}
		else {
			error = Error.WrongNumArgs;
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
			error = Error.WrongNumArgs;
		}
	}
	else if (cmd == "prog") {
		RunProgramEditor();
		ClearScreen();
	}
	else if (cmd == "list") {
		error = Error.NotImplemented;
	}
	else {
		syntaxError = true;
	}

	if (syntaxError) {
		PrintLine(Error.SyntaxError);
	}
	else if (!error.empty()) {
		PrintLine(error);
	}

	PrintLine("Ok");
}

void SetWorkingDir(std::string dir)
{
	if (File::Exists(dir)) {
		Shell.WorkingDir = dir;
	}
	else {
		PrintLine("Path not found");
	}
}

void PrintFiles(std::string pattern)
{
	auto files = File::List(Shell.WorkingDir, "*" + pattern, true);
	for (int i = 0; i < files.size(); i++) {
		std::string& file = files[i];
		if (file != "." && file != ".." && !File::IsDirectory(file)) {
			PrintLine(file);
			DrawScreenBuffer();
			UpdateEntireScreen();
		}
	}
}

struct {

	const int MaxCommandLength = 28;

	struct {
		int Line;
		int X;
		int Y;
	} Cursor;

	struct {
		int FirstLine;
		int LastLine;
	} View;

	struct {
		int LineNumber;
		int Command;
		int Params;
		int Label;
	} Color;

} PrgEdit;

void AddProgramLine(std::string command)
{
	ProgramLine* line = new ProgramLine();
	line->Command = command;
	Program.Lines.push_back(line);
}

void InitProgramEditor()
{
	PrgEdit.Cursor.Line = 0;
	PrgEdit.Cursor.X = 0;
	PrgEdit.Cursor.Y = 0;
	PrgEdit.View.FirstLine = 0;
	PrgEdit.View.LastLine = Screen.Buf->GetHeight() - 1;
	
	PrgEdit.Color.LineNumber = 0;
	PrgEdit.Color.Command = 27;
	PrgEdit.Color.Params = 1;
	PrgEdit.Color.Label = 43;

	if (Program.Lines.empty()) {
		AddProgramLine("");
	}
}

void RunProgramEditor()
{
	ClearScreen();

	bool progEditRunning = true;
	while (progEditRunning) {
		DrawScreenBorder();
		DrawScreenBuffer();
		PrintProgramEditor();
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
			if (key == SDLK_ESCAPE) {
				progEditRunning = false;
				return;
			}
			else if (key == SDLK_RETURN && alt) {
				ToggleFullscreen();
			}
			else if (key == SDLK_DOWN) {
				if (ctrl) {
					if (PrgEdit.View.LastLine < Program.Lines.size() - 1) {
						PrgEdit.View.FirstLine++;
						PrgEdit.View.LastLine++;
						PrgEdit.Cursor.Line++;
						ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
						if (PrgEdit.Cursor.X >= line->Command.length()) {
							PrgEdit.Cursor.X = line->Command.length();
						}
					}
				}
				else {
					if (PrgEdit.Cursor.Y < Screen.Buf->GetHeight() - 1 && 
						PrgEdit.Cursor.Line < Program.Lines.size() - 1) {

						PrgEdit.Cursor.Y++;
						PrgEdit.Cursor.Line++;
						ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
						if (PrgEdit.Cursor.X >= line->Command.length()) {
							PrgEdit.Cursor.X = line->Command.length();
						}
					}
					else {
						if (PrgEdit.View.LastLine < Program.Lines.size() - 1) {
							PrgEdit.View.FirstLine++;
							PrgEdit.View.LastLine++;
							PrgEdit.Cursor.Line++;
							ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
							if (PrgEdit.Cursor.X >= line->Command.length()) {
								PrgEdit.Cursor.X = line->Command.length();
							}
						}
					}
				}
			}
			else if (key == SDLK_UP) {
				if (ctrl) {
					if (PrgEdit.View.FirstLine > 0) {
						PrgEdit.View.FirstLine--;
						PrgEdit.View.LastLine--;
						PrgEdit.Cursor.Line--;
						ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
						if (PrgEdit.Cursor.X >= line->Command.length()) {
							PrgEdit.Cursor.X = line->Command.length();
						}
					}
				}
				else {
					if (PrgEdit.Cursor.Y > 0) {
						PrgEdit.Cursor.Y--;
						PrgEdit.Cursor.Line--;
						ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
						if (PrgEdit.Cursor.X >= line->Command.length()) {
							PrgEdit.Cursor.X = line->Command.length();
						}
					}
					else {
						if (PrgEdit.View.FirstLine > 0) {
							PrgEdit.View.FirstLine--;
							PrgEdit.View.LastLine--;
							PrgEdit.Cursor.Line--;
							ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
							if (PrgEdit.Cursor.X >= line->Command.length()) {
								PrgEdit.Cursor.X = line->Command.length();
							}
						}
					}
				}
			}
			else if (key == SDLK_LEFT) {
				if (PrgEdit.Cursor.X > 0) {
					PrgEdit.Cursor.X--;
				}
			}
			else if (key == SDLK_RIGHT) {
				ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
				if (PrgEdit.Cursor.X < line->Command.length()) {
					PrgEdit.Cursor.X++;
				}
			}
			else if (key == SDLK_HOME) {
				if (ctrl) {
					PrgEdit.Cursor.X = 0;
					PrgEdit.Cursor.Y = 0;
					PrgEdit.Cursor.Line = 0;
					PrgEdit.View.FirstLine = 0;
					PrgEdit.View.LastLine = Screen.Buf->GetHeight() - 1;
				}
				else {
					PrgEdit.Cursor.X = 0;
				}
			}
			else if (key == SDLK_END) {
				if (ctrl) {
					PrgEdit.Cursor.Line = Program.Lines.size() - 1;
					ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
					PrgEdit.Cursor.X = line->Command.length();

					if (Program.Lines.size() >= Screen.Buf->GetHeight()) {
						PrgEdit.Cursor.Y = Screen.Buf->GetHeight() - 1;
						PrgEdit.View.FirstLine = Program.Lines.size() - Screen.Buf->GetHeight();
						PrgEdit.View.LastLine = PrgEdit.View.FirstLine + Screen.Buf->GetHeight() - 1;
					}
					else {
						PrgEdit.Cursor.Y = PrgEdit.Cursor.Line;
					}
				}
				else {
					ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
					PrgEdit.Cursor.X = line->Command.length();
				}
			}
			else if (key == SDLK_BACKSPACE) {
				ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
				if (PrgEdit.Cursor.X > 0) {
					PrgEdit.Cursor.X--;
					line->Command.erase(line->Command.begin() + PrgEdit.Cursor.X);
				}
			}
			else if (key == SDLK_DELETE) {
				ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
				if (line->Command.empty()) {
					if (PrgEdit.Cursor.Line < Program.Lines.size() - 1) {
						Program.Lines.erase(Program.Lines.begin() + PrgEdit.Cursor.Line);
						delete line;
						line = NULL;
					}
				}
				else {
					line->Command.erase(line->Command.begin() + PrgEdit.Cursor.X);
				}
			}
			else if (key == SDLK_RETURN) {
				ProgramLine* currentLine = Program.Lines[PrgEdit.Cursor.Line];
				ProgramLine* newline = new ProgramLine();
				
				if (PrgEdit.Cursor.Y < Screen.Buf->GetHeight() - 1) {
					PrgEdit.Cursor.Line++;
					PrgEdit.Cursor.Y++;
				}
				else {
					PrgEdit.Cursor.Line++;
					PrgEdit.View.FirstLine++;
					PrgEdit.View.LastLine++;
				}

				if (PrgEdit.Cursor.X == 0) {
					newline->Command = currentLine->Command.substr(PrgEdit.Cursor.X);
					currentLine->Command = "";
				}

				Program.Lines.insert(Program.Lines.begin() + PrgEdit.Cursor.Line, newline);

				PrgEdit.Cursor.X = 0;

				FormatProgramLine(currentLine, true);
			}
			else if (IsTypableChar(key)) {
				int ch = (int)key;
				if (shift) {
					ch = String::ShiftChar(ch);
				}
				if (Key::CapsLock() || shift) {
					ch = toupper(ch);
				}
				ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
				if (PrgEdit.Cursor.X < line->Command.length()) {
					line->Command[PrgEdit.Cursor.X] = ch;
				}
				else if (line->Command.length() < PrgEdit.MaxCommandLength) {
					line->Command.push_back(ch);
				}
				if (PrgEdit.Cursor.X < line->Command.length()) {
					PrgEdit.Cursor.X++;
				}
				if (ch == SpecialChar.Space) {
					FormatProgramLine(line, false);
				}
			}
		}
	}
}

void FormatProgramLine(ProgramLine* line, bool trim)
{
	if (trim) {
		line->Command = String::Trim(line->Command);
	}

	if (String::EndsWith(line->Command, ":")) {
		return;
	}

	int spaceIndex = line->Command.find_first_of(SpecialChar.Space);

	if (spaceIndex > 0) {
		for (int i = 0; i < spaceIndex; i++) {
			line->Command[i] = toupper(line->Command[i]);
		}
	}
	else {
		line->Command = String::ToUpper(line->Command);
	}
}

void PrintProgramEditor()
{
	int row = 0;
	int col1 = 1;

	for (int i = PrgEdit.View.FirstLine; i <= PrgEdit.View.LastLine; i++) {
		ClearLine(row);
		if (i < Program.Lines.size()) {
			ProgramLine* line = Program.Lines[i];
			std::string command = line->Command;
			
			if (String::EndsWith(String::Trim(command), ":")) {
				Print(command, col1, row, PrgEdit.Color.Label, Screen.Color.Bg);
			}
			else {
				int spaceIndex = command.find_first_of(' ');
				if (spaceIndex > 0) {
					std::string name = command.substr(0, spaceIndex);
					std::string params = command.substr(spaceIndex);
					Print(name, col1, row, PrgEdit.Color.Command, Screen.Color.Bg);
					Print(params, col1 + name.length(), row, PrgEdit.Color.Params, Screen.Color.Bg);
				}
				else {
					Print(command, col1, row, PrgEdit.Color.Command, Screen.Color.Bg);
				}
			}
		}
		row++;
	}

	int csrX = PrgEdit.Cursor.X + col1;
	int csrY = PrgEdit.Cursor.Y;
	
	Object* o = Screen.Buf->GetObject(csrX, csrY, 0);
	ObjectChar& ch = o->GetAnimation().GetFrame(0);
	int temp = ch.BackColorIx;
	ch.BackColorIx = ch.ForeColorIx;
	ch.ForeColorIx = temp;

	std::string info = String::Format("%i/%i",
		PrgEdit.Cursor.Line + 1, Program.Lines.size());

	PrintOnBottomBorder(info);
}
