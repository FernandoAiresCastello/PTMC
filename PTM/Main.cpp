#include <SDL.h>
#include <TBRLGPT.h>
#include "Functions.h"
using namespace TBRLGPT;

int main(int argc, char** argv)
{
	InitScreen();
	PrintIntro();

	bool running = true;
	while (running) {
		DrawScreenBorder();
		DrawScreenBuffer();
		UpdateCursor();
		UpdateEntireScreen();

		SDL_Event e = { 0 };
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			running = false;
			break;
		}
		else if (e.type == SDL_KEYDOWN) {
			SDL_Keycode key = e.key.keysym.sym;
			bool alt = Key::Alt();
			bool ctrl = Key::Ctrl();
			bool shift = Key::Shift();

			if (key == SDLK_ESCAPE) {
				running = false;
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
			else {
				int ch = (int)key;
				if (ch >= 32 && ch < 256) {
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

	return 0;
}
