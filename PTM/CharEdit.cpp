#include "CharEdit.h"
#include "Ui.h"

CharEdit::CharEdit(Graphics* gr, Datafile* data)
{
	Gr = gr;
	Data = data;
	X = 1;
	Y = 1;
	CursorX = 0;
	CursorY = 0;
	Running = false;
	InitPixelBuffer();
}

CharEdit::~CharEdit()
{
}

void CharEdit::Draw()
{
	Ui::DrawWindow(Data, Gr, X, Y, 8, 8, ForeColor, BackColor);

	const int bit0 = 250;
	const int bit1 = 219;
	const int selBit0 = 250;
	const int selBit1 = 254;
	const int fgc = Data->GetPalette()->Get(ForeColor)->ToInteger();
	const int bgc = Data->GetPalette()->Get(BackColor)->ToInteger();

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

void CharEdit::InitPixelBuffer()
{
	byte* pixels = Data->GetCharset()->Get(CharIndex);
	int bufferIx = 0;

	for (int row = 0; row < Char::Height; row++) {
		const unsigned int& bits = pixels[row];
		for (int pos = Char::Width - 1; pos >= 0; pos--, bufferIx++) {
			int pixelOn = (bits & (1 << pos));
			Buffer[bufferIx] = pixelOn ? '1' : '0';
			OriginalPixels[bufferIx] = Buffer[bufferIx];
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

void CharEdit::RevertChar()
{
	for (int i = 0; i < 64; i++)
		Buffer[i] = OriginalPixels[i];
	
	UpdateChar();
}

void CharEdit::HandleEvents()
{
	bool alt = Key::Alt();
	bool shift = Key::Shift();

	SDL_Event e = { 0 };
	SDL_PollEvent(&e);
	if (e.type == SDL_QUIT) {
		Running = false;
	}
	if (e.type == SDL_KEYDOWN) {
		SDL_Keycode key = e.key.keysym.sym;
		if (key == SDLK_ESCAPE) {
			Running = false;
			RevertChar();
		}
		else if (key == SDLK_RETURN) {
			if (alt) {
				Gr->ToggleFullscreen();
				Gr->Update();
			}
			else {
				Running = false;
			}
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
