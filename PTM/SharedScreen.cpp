#include "SharedScreen.h"
#include "SharedObjects.h"

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

void InitSharedScreen()
{
	const int width = 160;
	const int height = 144;
	const int zoom = 4;

	Screen.Gr = new Graphics(width, height, zoom * width, zoom * height, false);
	Screen.Gr->SetWindowTitle(APPLICATION_NAME);
	Screen.Cols = Screen.Gr->Cols;
	Screen.Rows = Screen.Gr->Rows;
	Screen.Ctx = new UIContext(Screen.Gr, 0xffffff, 0xff0000);
	Screen.Chr = Screen.Ctx->Chars;
	Screen.Pal = Screen.Ctx->Pal;
	Screen.Ctx->Clear();
	Screen.Chr->Clear();
	Project* proj = GetSharedProject();
	Screen.Ctx->Chars = proj->GetCharset();
	Screen.Ctx->Pal = proj->GetPalette();
	Screen.Buf = new Map(proj, "main", Screen.Cols - 2, Screen.Rows - 2, 2);
	Screen.View = new MapViewport(
		Screen.Ctx, Screen.Buf, 1, 1,
		Screen.Buf->GetWidth(), Screen.Buf->GetHeight(), 0, 0, 200);

	ShowCursor(true);
	Screen.Cursor.Insert = false;
	Locate(0, 0);
	SetColors(DefaultColor::Fore, DefaultColor::Back, DefaultColor::Border);
	ClearScreen();
	DrawScreenBuffer();
	DrawScreenBorder();
	UpdateEntireScreen();
}

void DestroySharedScreen()
{
	Screen.Ctx->Chars = NULL;
	Screen.Ctx->Pal = NULL;
	delete Screen.Ctx;
	Screen.Ctx = NULL;
	delete Screen.Gr;
	Screen.Gr = NULL;
}

Graphics* GetSharedGraphics()
{
	return Screen.Gr;
}

Map* GetSharedScreenBuffer()
{
	return Screen.Buf;
}

int GetScreenBufferWidth()
{
	return Screen.Buf->GetWidth();
}

int GetScreenBufferHeight()
{
	return Screen.Buf->GetHeight();
}

int GetScreenBackColor()
{
	return Screen.Color.Bg;
}

int GetScreenForeColor()
{
	return Screen.Color.Fg;
}

int GetScreenBorderColor()
{
	return Screen.Color.Border;
}

void ToggleFullscreen()
{
	Screen.Gr->ToggleFullscreen();
}

void ToggleInsertMode()
{
	Screen.Cursor.Insert = !Screen.Cursor.Insert;
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
		if (chr != SpecialChar::Null) {
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

void PrintOnBottomRightBorder(std::string text)
{
	PrintOnBottomRightBorder(text, Screen.Color.Fg, Screen.Color.Border);
}

void PrintOnBottomLeftBorder(std::string text)
{
	PrintOnBottomLeftBorder(text, Screen.Color.Fg, Screen.Color.Border);
}

void PrintOnBottomRightBorder(std::string text, int fgc)
{
	PrintOnBottomRightBorder(text, fgc, Screen.Color.Border);
}

void PrintOnBottomLeftBorder(std::string text, int fgc)
{
	PrintOnBottomLeftBorder(text, fgc, Screen.Color.Border);
}

void PrintOnBottomRightBorder(std::string text, int fgc, int bgc)
{
	Screen.Ctx->ForeColor = Screen.Pal->Get(fgc)->ToInteger();
	Screen.Ctx->BackColor = Screen.Pal->Get(bgc)->ToInteger();
	Screen.Ctx->Print(Screen.Gr->Cols - text.length(), Screen.Gr->Rows - 1, text);
}

void PrintOnBottomLeftBorder(std::string text, int fgc, int bgc)
{
	Screen.Ctx->ForeColor = Screen.Pal->Get(fgc)->ToInteger();
	Screen.Ctx->BackColor = Screen.Pal->Get(bgc)->ToInteger();
	Screen.Ctx->Print(0, Screen.Gr->Rows - 1, text);
}

void PutChar(int ch)
{
	if (Screen.Cursor.Insert) {
		std::string line = GetCurrentLine();
		line.insert(line.begin() + Screen.Cursor.X, SpecialChar::Space);
		line = line.substr(0, Screen.Buf->GetWidth() - 1);
		Print(line, 0, Screen.Cursor.Y);
	}

	if (Screen.Cursor.X < Screen.Buf->GetWidth() - 1) {
		ObjectChar* under = GetCharUnderCursor();
		under->Index = ch;
		CursorMove(1, 0);
	}
}

void PutChar(int ch, int x, int y, int fgc, int bgc)
{
	ObjectChar& oc = Screen.Buf->GetObject(x, y, 0)->GetAnimation().GetFrame(0);
	oc.Index = ch;
	oc.ForeColorIx = fgc;
	oc.BackColorIx = bgc;
}

Object* GetScreenBufferObject(int x, int y)
{
	return Screen.Buf->GetObject(x, y, 0);
}

bool IsTypableChar(int ch)
{
	return ch >= TypableChar::Minimum && ch <= TypableChar::Maximum;
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
				anim.AddFrame(ObjectChar(SpecialChar::Insert, Screen.Color.Fg, Screen.Color.Bg));
			}
			else {
				anim.AddFrame(ObjectChar(chr->Index, Screen.Color.Bg, Screen.Color.Fg));
			}

			if (chr->Index != SpecialChar::Null && chr->Index != SpecialChar::Space) {
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

	for (int x = Screen.Buf->GetWidth() - 1; x >= 0; x--) {
		ObjectChar* ch = GetCharAt(x, Screen.Cursor.Y);
		if (ch->Index != SpecialChar::Null) {
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
