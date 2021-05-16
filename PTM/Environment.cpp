#include "Environment.h"

Environment::Environment()
{
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
}

Environment::~Environment()
{
}

void Environment::SetVar(std::string var, std::string value)
{
	Vars[var] = value;
}

void Environment::SetVar(std::string var, int value)
{
	Vars[var] = String::ToString(value);
}

std::string Environment::GetStringVar(std::string var)
{
	return Vars[var];
}

int Environment::GetNumericVar(std::string var)
{
	return String::ToInt(GetStringVar(var));
}
