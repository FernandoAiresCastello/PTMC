#pragma once

#include <map>
#include <string>
#include <vector>
#include <stack>
#include <TBRLGPT.h>
#include "Datafile.h"
using namespace TBRLGPT;

class EditorBase
{
public:
	EditorBase(Graphics* gr, Datafile* data);
	virtual ~EditorBase();

	void Run();
	void SetForeColor(int color);
	void SetBackColor(int color);
	void SetBorderColor(int color);
	void SetBorderTextColor(int color);
	int GetForeColor();
	int GetBackColor();
	int GetBorderColor();
	int GetBorderTextColor();

protected:
	Graphics* Gr;
	Datafile* Data;
	bool Running;
	Scene* Scrbuf;
	SceneView* View;
	int BorderColor;
	int ForeColor;
	int BackColor;
	int BorderTextColor;
	SceneObject* Cursor;
	bool Insert;
	bool AllowExitOnEscape;
	bool AllowClearBuffer;
	std::string TopBorderText;
	std::string BottomBorderText;
	SceneObject* Clipboard;
	std::vector<ObjectChar> TileStack;
	Position CharEditPos;
	Position CharSelectPos;

	virtual void OnStart();
	virtual void OnLoop();
	virtual void TypeEnter();

	void Draw();
	void FollowCursorInsideView();
	void UpdateCursor();
	ObjectChar& GetCursorFrame(int frame);
	ObjectChar& GetObjectFrame(SceneObject* o, int frame);
	ObjectChar& GetObjectFrame(Object* o, int frame);
	void OnKeyPress(SDL_Keycode key);
	void TypePlainChar(int ch);
	void TypeObject(Object o);
	void TypeBackspace();
	void TypeDelete();
	void TypeTab();
	void DeleteCharUnderCursor();
	void DeleteChar(int x, int y);
	void TypeString(std::string str);
	void TypeStringCrlf(std::string str);
	void TypeTemplate(std::string id);
	void TypeTileStack();
	void PutPlainChar(int ch, int x, int y);
	void Crlf();
	void ClearScreen();
	std::vector<SceneObject*> GetObjectsInLine(int line, int column);
	std::vector<SceneObject*> GetObjectsInLine(int line);
	std::string GetStringInLine(int line);
	int GetLastXInLine(int line);
	int GetLastX();
	int GetLastY();
	SceneObject* GetObject(int x, int y);
	SceneObject* GetObjectUnderCursor();
	std::string GetObjectType(int x, int y);
	bool HasObject(int x, int y);
	void PrintOnTopBorder();
	void PrintOnBottomBorder();
	void CopyChar();
	void SetClipboardTile(int ch, int fgc, int bgc);
	void PasteChar();
	void ClearClipboard();
	void PushTile(int ix, int fgc, int bgc);
	void SetObjTemplate(std::string id);
	void DeleteObjTemplate(std::string id);
	void EditChar(Graphics* gr, Datafile* data, int ch);
	int SelectChar(Graphics* gr, Datafile* data);
};
