#include "CharEdit.h"

int CharEdit::X;
int CharEdit::Y;

CharEdit::CharEdit(EditorBase* baseEditor, Graphics* gr, Datafile* data, int ch)
{
	BaseEditor = baseEditor;
	Gr = gr;
	Data = data;
	Running = false;
	CharIndex = ch;
	CursorX = 0;
	CursorY = 0;
	UpdateBuffer();
}

CharEdit::~CharEdit()
{
}

void CharEdit::Draw()
{
	const int h = 196;
	const int v = 179;
	const int tl = 218;
	const int tr = 191;
	const int bl = 192;
	const int br = 217;
	const int bit0 = 250;
	const int bit1 = 219;
	const int selBit0 = 250;
	const int selBit1 = 254;
	const int fgc = Data->GetPalette()->Get(BaseEditor->GetForeColor())->ToInteger();
	const int bgc = Data->GetPalette()->Get(BaseEditor->GetBackColor())->ToInteger();

	Gr->PutChar(Data->GetCharset(), tl, X + 0, Y + 0, fgc, bgc);
	Gr->PutChar(Data->GetCharset(), tr, X + 9, Y + 0, fgc, bgc);
	Gr->PutChar(Data->GetCharset(), bl, X + 0, Y + 9, fgc, bgc);
	Gr->PutChar(Data->GetCharset(), br, X + 9, Y + 9, fgc, bgc);
	for (int x = 1; x <= 8; x++) {
		Gr->PutChar(Data->GetCharset(), h, X + x, Y + 0, fgc, bgc);
		Gr->PutChar(Data->GetCharset(), h, X + x, Y + 9, fgc, bgc);
	}
	for (int y = 1; y <= 8; y++) {
		Gr->PutChar(Data->GetCharset(), v, X + 0, Y + y, fgc, bgc);
		Gr->PutChar(Data->GetCharset(), v, X + 9, Y + y, fgc, bgc);
	}

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			int pixelOn = Buffer[x + (y * Char::Width)] == '1';
			if (CursorX == x && CursorY == y) {
				if (pixelOn)
					Gr->PutChar(Data->GetCharset(), selBit1, X + x + 1, Y + y + 1, bgc, fgc);
				else
					Gr->PutChar(Data->GetCharset(), selBit0, X + x + 1, Y + y + 1, bgc, fgc);
			}
			else {
				if (pixelOn)
					Gr->PutChar(Data->GetCharset(), bit1, X + x + 1, Y + y + 1, fgc, bgc);
				else
					Gr->PutChar(Data->GetCharset(), bit0, X + x + 1, Y + y + 1, fgc, bgc);
			}
		}
	}
}

void CharEdit::ToggleCurrentPixel()
{
	const int ix = CursorX + (CursorY * Char::Width);
	if (Buffer[ix] == '1')
		Buffer[ix] = '0';
	else if (Buffer[ix] == '0')
		Buffer[ix] = '1';

	UpdateChar();
}

void CharEdit::SetCurrentPixel(int value)
{
	Buffer[CursorX + (CursorY * Char::Width)] = value > 0 ? '1' : '0';
	UpdateChar();
}

void CharEdit::ClearChar()
{
	for (int i = 0; i < 64; i++)
		Buffer[i] = '0';
	
	UpdateChar();
}

void CharEdit::UpdateBuffer()
{
	byte* pixels = Data->GetCharset()->Get(CharIndex);
	int bufferIx = 0;

	for (int row = 0; row < Char::Height; row++) {
		const unsigned int& bits = pixels[row];
		for (int pos = Char::Width - 1; pos >= 0; pos--, bufferIx++) {
			int pixelOn = (bits & (1 << pos));
			Buffer[bufferIx] = pixelOn ? '1' : '0';
		}
	}
}

void CharEdit::UpdateChar()
{
	byte* pixels = Data->GetCharset()->Get(CharIndex);

	for (int row = 0; row < Char::Height; row++) {
		std::string rowbits = "";
		for (int col = 0; col < Char::Width; col++)
			rowbits += Buffer[col + (row * Char::Width)];

		pixels[row] = (byte)strtol(rowbits.c_str(), NULL, 2);
	}
}

void CharEdit::HandleKeyEvents()
{
	bool alt = Key::Alt();
	bool shift = Key::Shift();

	SDL_Event e = { 0 };
	SDL_PollEvent(&e);
	if (e.type == SDL_KEYDOWN) {
		SDL_Keycode key = e.key.keysym.sym;
		if (key == SDLK_ESCAPE || key == SDLK_RETURN) {
			Running = false;
		}
		else if (key == SDLK_RIGHT) {
			if (alt) {
				X++;
			}
			else {
				if (CursorX < 7)
					CursorX++;
				if (shift)
					SetCurrentPixel(1);
			}
		}
		else if (key == SDLK_LEFT) {
			if (alt) {
				X--;
			}
			else {
				if (CursorX > 0)
					CursorX--;
				if (shift)
					SetCurrentPixel(1);
			}
		}
		else if (key == SDLK_DOWN) {
			if (alt) {
				Y++;
			}
			else {
				if (CursorY < 7)
					CursorY++;
				if (shift)
					SetCurrentPixel(1);
			}
		}
		else if (key == SDLK_UP) {
			if (alt) {
				Y--;
			}
			else {
				if (CursorY > 0)
					CursorY--;
				if (shift)
					SetCurrentPixel(1);
			}
		}
		else if (key == SDLK_LSHIFT || key == SDLK_RSHIFT) {
			SetCurrentPixel(1);
		}
		else if (key == SDLK_SPACE) {
			ToggleCurrentPixel();
		}
		else if (key == SDLK_DELETE) {
			ClearChar();
		}
	}
}
