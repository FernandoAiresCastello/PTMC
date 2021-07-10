#pragma once
#include <string>
#include <vector>
#include <TBRLGPT.h>
using namespace TBRLGPT;

struct SpecialChar {
	static const int Null = 0;
	static const int Space = 32;
	static const int Insert = 127;
};

struct TypableChar {
	static const int Minimum = 32;
	static const int Maximum = 126;
};

struct DefaultColor {
	static const int Fore = 1;
	static const int Back = 52;
	static const int Border = 50;
};

void InitSharedScreen();
void DestroySharedScreen();
Graphics* GetSharedGraphics();
Map* GetSharedScreenBuffer();
int GetScreenBufferWidth();
int GetScreenBufferHeight();
int GetScreenBackColor();
int GetScreenForeColor();
int GetScreenBorderColor();
void ToggleFullscreen();
void ToggleInsertMode();
void DrawScreenBuffer();
void DrawScreenBorder();
void UpdateEntireScreen();
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
void Print(std::string text, int x, int y, int fgc, int bgc);
void PrintLine(std::string text);
void PrintOnBottomRightBorder(std::string text);
void PrintOnBottomLeftBorder(std::string text);
void PrintOnBottomRightBorder(std::string text, int fgc);
void PrintOnBottomLeftBorder(std::string text, int fgc);
void PrintOnBottomRightBorder(std::string text, int fgc, int bgc);
void PrintOnBottomLeftBorder(std::string text, int fgc, int bgc);
void PutChar(int ch);
void PutChar(int ch, int x, int y, int fgc, int bgc);
Object* GetScreenBufferObject(int x, int y);
bool IsTypableChar(int ch);
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
Graphics* GetScreenGraphics();
