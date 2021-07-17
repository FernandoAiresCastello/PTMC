#pragma once

#include <map>
#include <string>
#include <vector>
#include <stack>
#include <TBRLGPT.h>
#include "Datafile.h"
using namespace TBRLGPT;

class CommandShell
{
public:
	CommandShell(Graphics* gr, Datafile* data);
	~CommandShell();

	void Run();

private:
	Graphics* Gr;
	Datafile* Data;
	bool Running;
	Scene* Scrbuf;
	SceneView* View;
	int BorderColor;
	int ForeColor;
	int BackColor;
	SceneObject* Cursor;
	bool Insert;

	void Draw();
	void SetForeColor(int color);
	void SetBackColor(int color);
	void SetBorderColor(int color);
	void UpdateCursor();
	void OnKeyPress(SDL_Keycode key);
	void TypePlainChar(int ch);
	void TypeBackspace();
	void TypeDelete();
	void DeleteChar(int x, int y);
	void TypeString(std::string str);
	void TypeStringCrlf(std::string str);
	void PutPlainChar(int ch, int x, int y);
	void Crlf();
	void ClearScreen();
	std::vector<SceneObject*> GetObjectsInLine(int line, int column);
	std::vector<SceneObject*> GetObjectsInLine(int line);
	std::string GetStringInLine(int line);
	int GetLastXInLine(int line);
	SceneObject* GetObject(int x, int y);
	std::string GetObjectType(int x, int y);
	bool HasObject(int x, int y);
	void PrintIntro();
	void Ok();
	void TypeEnter();
	void InterpretLine(std::string line);
};
