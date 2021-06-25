#pragma once
#include <string>
#include <vector>
#include <TBRLGPT.h>
using namespace TBRLGPT;

struct Command {
	std::string Name;
	std::vector<std::string> Args;
	bool Empty;
};

void InitScreen();
void Debug();
void ToggleFullscreen();
void DrawScreenBuffer();
void DrawScreenBorder();
void UpdateEntireScreen();
void PrintIntro();
void ToggleInsertMode();
void Locate(int x, int y);
void SetColors(int fg, int bg, int border);
void SetForeColor(int color);
void SetBackColor(int color);
void SetBorderColor(int color);
void UpdateBufferColors();
void ClearScreen();
void ClearLine(int index);
std::string GetCurrentLine();
std::string GetCurrentLineTrimmed();
std::string GetLine(int index);
void Print(std::string text);
void Print(std::string text, int x, int y);
void PrintLine(std::string text);
void PutChar(int ch);
void ShowCursor(bool show);
void UpdateCursor();
ObjectChar* GetCharUnderCursor();
ObjectChar* GetCharAt(int x, int y);
void Crlf();
void BackSpace();
void DeleteCharUnderCursor();
void CursorHome();
void CursorEnd();
void CursorMove(int dx, int dy);
void CursorMoveToStartOfLine();
void CursorMoveToEndOfLine();
void ScrollBufferUp();
Command ParseCommandLine(std::string commandLine);
void InterpretCurrentLine();
