#include "CharSelect.h"
#include "Ui.h"

CharSelect::CharSelect(Graphics* gr, Datafile* data)
{
	Gr = gr;
	Data = data;
	X = 1;
	Y = 1;
	Width = 16;
	Height = 16;
	CharsPerRow = 16;
	CharScene = new Scene();
	InitChars();
	View = new SceneView(gr, Data->GetCharset(), Data->GetPalette(), 250);
	View->SetScene(CharScene);
	Cancelled = false;
}

CharSelect::~CharSelect()
{
	delete View;
	delete CharScene;
}

void CharSelect::Draw()
{
	UpdateCursor();
	CharScene->SetBackObject(ObjectChar(0, ForeColor, BackColor));
	Ui::DrawWindow(Data, Gr, X, Y, Width, Height, ForeColor, BackColor);
	View->SetPosition(X + 1, Y + 1);
	View->SetSize(Width, Height);
	View->Draw();
}

void CharSelect::HandleEvents()
{
	bool alt = Key::Alt();
	bool ctrl = Key::Ctrl();
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
			Cancelled = true;
		}
		else if (key == SDLK_RETURN) {
			if (alt) {
				Gr->ToggleFullscreen();
				Gr->Update();
			}
			else {
				Running = false;
				Cancelled = false;
			}
		}
		else if (key == SDLK_RIGHT) {
			if (alt) {
				X++;
			}
			else {
				if (Cursor->GetX() < CharsPerRow - 1) {
					Cursor->Move(1, 0);
				}
			}
		}
		else if (key == SDLK_LEFT) {
			if (alt) {
				X--;
			}
			else {
				if (Cursor->GetX() > 0) {
					Cursor->Move(-1, 0);
				}
			}
		}
		else if (key == SDLK_DOWN) {
			if (alt) {
				Y++;
			}
			else {
				Cursor->Move(0, 1);
				if (Cursor->GetY() >= Height) {
					View->Scroll(0, 1);
				}
			}
		}
		else if (key == SDLK_UP) {
			if (alt) {
				Y--;
			}
			else {
				if (Cursor->GetY() > 0) {
					Cursor->Move(0, -1);
					if (Cursor->GetY() < View->GetScrollY()) {
						View->Scroll(0, -1);
					}
				}
			}
		}
	}
}

void CharSelect::InitChars()
{
	int x = 0;
	int y = 0;

	CharScene->Clear();

	for (int i = 0; i < Charset::Size; i++) {
		SceneObject* o = new SceneObject();
		o->SetPosition(x, y, 0);
		o->GetObj()->GetAnimation().Clear();
		o->GetObj()->GetAnimation().AddFrame(ObjectChar(i, ForeColor, BackColor));
		o->GetObj()->SetProperty("index", i);
		o->SetScene(CharScene);
		CharScene->AddObject(o);
		x++;
		if (x >= CharSelect::CharsPerRow) {
			x = 0;
			y++;
		}
	}

	Cursor = new SceneObject();
	Cursor->SetLayer(1);
	Cursor->SetScene(CharScene);
	Cursor->GetObj()->GetAnimation().Clear();
	Cursor->GetObj()->GetAnimation().AddFrame(ObjectChar(0, ForeColor, BackColor));
	Cursor->GetObj()->GetAnimation().AddFrame(ObjectChar(0, BackColor, ForeColor));
}

void CharSelect::UpdateCursor()
{
	SceneObject* o = Cursor->Under();

	if (o != NULL) {
		Cursor->GetObj()->GetAnimation().GetFrame(0).Index = o->GetObj()->GetAnimation().GetFrame(0).Index;
		Cursor->GetObj()->GetAnimation().GetFrame(1).Index = o->GetObj()->GetAnimation().GetFrame(0).Index;
	}
	else {
		Cursor->GetObj()->GetAnimation().GetFrame(0).Index = 0;
		Cursor->GetObj()->GetAnimation().GetFrame(1).Index = 0;
	}

	Cursor->GetObj()->GetAnimation().GetFrame(0).ForeColorIx = ForeColor;
	Cursor->GetObj()->GetAnimation().GetFrame(0).BackColorIx = BackColor;
	Cursor->GetObj()->GetAnimation().GetFrame(1).ForeColorIx = BackColor;
	Cursor->GetObj()->GetAnimation().GetFrame(1).BackColorIx = ForeColor;
}

int CharSelect::GetSelectedChar()
{
	SceneObject* under = Cursor->Under();
	return (under != NULL && !Cancelled) ? under->GetObj()->GetPropertyAsNumber("index") : -1;
}
