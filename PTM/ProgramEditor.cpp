#include "ProgramEditor.h"

#define KEYMAP(key,fn) Keymap[key] = &ProgramEditor::fn

ProgramEditor::ProgramEditor()
{
	Running = false;
	InitKeyMapping();
	Wnd = new TWindow(256, 192, 3, false);
}

ProgramEditor::~ProgramEditor()
{
	delete Wnd;
}

void ProgramEditor::Run()
{
	Running = true;
	
	//Wnd->Clear();

	while (Running) {
		Draw();
		Wnd->Update();
		SDL_Event e = { 0 };
		SDL_PollEvent(&e);
		
		if (e.type == SDL_QUIT) {
			Quit();
		}
		else if (e.type == SDL_KEYDOWN) {
			auto fn = Keymap[e.key.keysym.sym];
			if (fn)
				(this->*fn)();
		}
	}
}

void ProgramEditor::InitKeyMapping()
{
	KEYMAP(SDLK_ESCAPE, Quit);
}

void ProgramEditor::Draw()
{
}

void ProgramEditor::Quit()
{
	Running = false;
}
