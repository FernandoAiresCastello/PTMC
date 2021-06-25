#include <vector>
#include <string>
#include "Functions.h"

struct {
	Project* Proj;
} System;

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

struct {
	const int Null = 0;
	const int Space = 32;
	const int Insert = 127;
} SpecialChar;

struct {
	const std::string SyntaxError = "Syntax error";
	const std::string InvalidArgList = "Invalid argument list";
} Error;

void InitScreen()
{
	Screen.Gr = new Graphics(256, 192, 640, 480, false);
	Screen.Gr->SetWindowTitle("PTM - Programmable Tile Machine");
	Screen.Cols = Screen.Gr->Cols;
	Screen.Rows = Screen.Gr->Rows;
	Screen.Ctx = new UIContext(Screen.Gr, 0xffffff, 0xff0000);
	Screen.Chr = Screen.Ctx->Chars;
	Screen.Pal = Screen.Ctx->Pal;
	Screen.Ctx->Clear();
	Screen.Chr->Clear();
	System.Proj = new Project();
	System.Proj->Load("ptm.tgpro");
	Screen.Ctx->Chars = System.Proj->GetCharset();
	Screen.Ctx->Pal = System.Proj->GetPalette();
	Screen.Buf = new Map(System.Proj, "main", Screen.Cols - 2, Screen.Rows - 2, 2);
	Screen.View = new MapViewport(
		Screen.Ctx, Screen.Buf, 1, 1, 
		Screen.Buf->GetWidth(), Screen.Buf->GetHeight(), 0, 0, 200);

	ShowCursor(true);
	Screen.Cursor.Insert = false;
	Locate(0, 0);
	SetColors(2, 6, 5);
	ClearScreen();
	DrawScreenBuffer();
	DrawScreenBorder();
	UpdateEntireScreen();
}

void Debug()
{
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
	Screen.Buf->SetStringOfObjects(
		text, Screen.Color.Fg, Screen.Color.Bg, x, y, 0);
}

void PrintLine(std::string text)
{
	Print(text);
	Crlf();
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
	const int newX = Screen.Cursor.X + dx;
	const int newY = Screen.Cursor.Y + dy;

	if (newX < 0 || newY < 0 || newX >= Screen.Buf->GetWidth() || newY >= Screen.Buf->GetHeight()) {
		return;
	}

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

Command ParseCommandLine(std::string commandLine)
{
	Command cmd;
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
	Command cmdobj = ParseCommandLine(GetCurrentLineTrimmed());
	Crlf();
	if (cmdobj.Empty)
		return;

	std::string cmd = cmdobj.Name;
	auto args = cmdobj.Args;
	int argc = args.size();
	std::string error;
	bool syntaxError = false;

	if (cmd == "debug") {
		PrintLine("Hello World!");
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
			error = Error.InvalidArgList;
		}
	}
	else if (cmd == "fgcolor") {
		if (argc == 1) {
			int fg = String::ToInt(args[0]);
			SetForeColor(fg);
		}
		else {
			error = Error.InvalidArgList;
		}
	}
	else if (cmd == "bgcolor") {
		if (argc == 1) {
			int bg = String::ToInt(args[0]);
			SetBackColor(bg);
		}
		else {
			error = Error.InvalidArgList;
		}
	}
	else if (cmd == "bdcolor") {
		if (argc == 1) {
			int border = String::ToInt(args[0]);
			SetBorderColor(border);
		}
		else {
			error = Error.InvalidArgList;
		}
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
