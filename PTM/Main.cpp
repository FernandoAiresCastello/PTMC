#include <SDL.h>
#include <TBRLGPT.h>
#include "Util.h"
#include "Program.h"

using namespace TBRLGPT;

#define PTM_INI_FILENAME "ptm.ini"

int main(int argc, char** argv)
{
	if (!File::Exists(PTM_INI_FILENAME)) {
		ShowErrorMessageBox("Initialization file " PTM_INI_FILENAME " not found.");
		return 1;
	}

	std::string programFile = String::RemoveAll(File::ReadText(PTM_INI_FILENAME), "\n\t ");
	if (!File::Exists(programFile)) {
		ShowErrorMessageBox("Program file " + programFile + " not found.");
		return 1;
	}

	Program program;
	std::vector<std::string> rawProgramLines = File::ReadLines(programFile);
	program.Parse(rawProgramLines);
	
	std::string validation = program.Validate();
	if (!validation.empty()) {
		ShowErrorMessageBox(validation);
		return 1;
	}

	/*
	Graphics* gr = new Graphics(256, 192, 800, 600, false);
	UIContext* ctx = new UIContext(gr, 0xe0e0e0, 0x102080);
	Project* proj = new Project();
	//proj->Load("test.tgpro");
	//Map* map = proj->GetMaps()[0];
	Map* map = new Map(proj, "test", 32, 24, 1);
	map->SetBackColor(1);
	MapViewport* mview = new MapViewport(ctx, map, 1, 3, 30, 20, 0, 0, 100);

	ctx->Clear();
	ctx->Print(1, 1, "Hello world!");

	bool running = true;
	while (running) {
		SDL_Event e = { 0 };
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			running = false;
		}
		else if (e.type == SDL_KEYDOWN) {
			SDL_Keycode key = e.key.keysym.sym;
			switch (key) {
				case SDLK_ESCAPE:
					running = false;
					break;
				default:
					break;
			}
		}

		mview->Draw();
		ctx->Update();
	}

	delete mview;
	delete proj;
	delete ctx;
	delete gr;
	*/

	return 0;
}
