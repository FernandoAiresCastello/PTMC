#include <algorithm>
#include "EditorBase.h"

#define CHAR_LAYER 0

EditorBase::EditorBase(Graphics* gr, Datafile* data)
{
	Gr = gr;
	Data = data;
	Running = false;
	Scrbuf = new Scene();
	Cursor = new SceneObject();
	Cursor->SetScene(Scrbuf);
	Cursor->SetLayer(1);
	View = new SceneView(Gr, data->GetCharset(), data->GetPalette(), 400);
	View->SetScene(Scrbuf);
	View->SetPosition(1, 1);
	View->SetSize(Gr->Cols - 2, Gr->Rows - 2);
	SetForeColor(1);
	SetBackColor(52);
	SetBorderColor(50);
	Cursor->GetObj()->GetAnimation().Clear();
	Cursor->GetObj()->GetAnimation().AddFrame(ObjectChar(0, ForeColor, BackColor));
	Cursor->GetObj()->GetAnimation().AddFrame(ObjectChar(0, BackColor, ForeColor));
	Insert = false;
}

EditorBase::~EditorBase()
{
	delete View;
	delete Scrbuf;
}

void EditorBase::Run()
{
	OnStart();
	Running = true;
	while (Running) {
		FollowCursorInsideView();
		UpdateCursor();
		Draw();
		Gr->Update();

		SDL_Event e = { 0 };
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			Running = false;
			break;
		}
		else if (e.type == SDL_KEYDOWN) {
			OnKeyPress(e.key.keysym.sym);
		}
	}
}

void EditorBase::OnStart()
{
}

void EditorBase::TypeEnter()
{
}

void EditorBase::Draw()
{
	Gr->Clear(Data->GetPalette()->Get(BorderColor)->ToInteger());
	View->Draw();
}

void EditorBase::SetForeColor(int color)
{
	ForeColor = color;
	for (auto& it : Scrbuf->GetObjects()) {
		Object* o = it.second->GetObj();
		if (o->HasPropertyValue("type", "plain")) {
			GetObjectFrame(o, 0).ForeColorIx = color;
		}
	}
}

void EditorBase::SetBackColor(int color)
{
	BackColor = color;
	Scrbuf->SetBackObject(ObjectChar(0, color, color));
	for (auto& it : Scrbuf->GetObjects()) {
		Object* o = it.second->GetObj();
		if (o->HasPropertyValue("type", "plain")) {
			GetObjectFrame(o, 0).BackColorIx = color;
		}
	}
}

void EditorBase::SetBorderColor(int color)
{
	BorderColor = color;
}

void EditorBase::FollowCursorInsideView()
{
	const int maxX = View->GetScrollX() + View->GetWidth() - 1;
	const int maxY = View->GetScrollY() + View->GetHeight() - 1;

	if (Cursor->GetX() > maxX) {
		View->Scroll(1, 0);
	}
	if (Cursor->GetY() > maxY) {
		View->Scroll(0, 1);
	}
}

void EditorBase::UpdateCursor()
{
	SceneObject* o = Cursor->Under();

	if (o != NULL) {
		if (Insert)
			GetCursorFrame(0).Index = 220;
		else
			GetCursorFrame(0).Index = GetObjectFrame(o, 0).Index;

		GetCursorFrame(1).Index = GetObjectFrame(o, 0).Index;
	}
	else {
		if (Insert)
			GetCursorFrame(0).Index = 220;
		else
			GetCursorFrame(0).Index = 0;

		GetCursorFrame(1).Index = 0;
	}

	GetCursorFrame(0).ForeColorIx = ForeColor;
	GetCursorFrame(0).BackColorIx = BackColor;
	GetCursorFrame(1).ForeColorIx = BackColor;
	GetCursorFrame(1).BackColorIx = ForeColor;
}

ObjectChar& EditorBase::GetCursorFrame(int frame)
{
	return GetObjectFrame(Cursor, frame);
}

ObjectChar& EditorBase::GetObjectFrame(SceneObject* o, int frame)
{
	return o->GetObj()->GetAnimation().GetFrame(frame);
}

ObjectChar& EditorBase::GetObjectFrame(Object* o, int frame)
{
	return o->GetAnimation().GetFrame(frame);
}

void EditorBase::OnKeyPress(SDL_Keycode key)
{
	bool shift = Key::Shift();
	bool ctrl = Key::Ctrl();
	bool alt = Key::Alt();

	switch (key) {
	case SDLK_RETURN: {
		if (alt) {
			Gr->ToggleFullscreen();
			Gr->Update();
		}
		else {
			TypeEnter();
		}
		break;
	}
	case SDLK_RIGHT: {
		if (alt) {
			View->Scroll(1, 0);
		}
		else {
			Cursor->Move(1, 0);
		}
		break;
	}
	case SDLK_LEFT: {
		if (alt) {
			if (View->GetScrollX() > 0)
				View->Scroll(-1, 0);
		}
		else {
			if (Cursor->GetX() > 0)
				Cursor->Move(-1, 0);
		}
		break;
	}
	case SDLK_DOWN: {
		if (alt) {
			View->Scroll(0, 1);
		}
		else {
			Cursor->Move(0, 1);
		}
		break;
	}
	case SDLK_UP: {
		if (alt) {
			if (View->GetScrollY() > 0)
				View->Scroll(0, -1);
		}
		else {
			if (Cursor->GetY() > 0)
				Cursor->Move(0, -1);
		}
		break;
	}
	case SDLK_HOME: {
		if (shift) {
			ClearScreen();
		}
		if (ctrl) {
			Cursor->SetPosition(0, 0);
			View->SetScroll(0, 0);
		}
		else {
			Cursor->SetX(0);
			View->SetScroll(0, View->GetScrollY());
		}
		break;
	}
	case SDLK_END: {
		if (ctrl) {
			int lastY = GetLastY();
			if (lastY < 0)
				break;
			Cursor->SetY(lastY);
			if (Cursor->GetY() > Gr->Rows - 3) {
				while (Cursor->GetY() != View->GetScrollY()) {
					View->Scroll(0, 1);
				}
				for (int i = 0; i < Gr->Rows - 3; i++) {
					View->Scroll(0, -1);
				}
			}
			int lastX = GetLastXInLine(Cursor->GetY());
			Cursor->SetX(lastX + 1);
			if (Cursor->GetX() > Gr->Cols - 3) {
				while (Cursor->GetX() != View->GetScrollX()) {
					View->Scroll(1, 0);
				}
				for (int i = 0; i < Gr->Cols - 3; i++) {
					View->Scroll(-1, 0);
				}
			}
		}
		else {
			int lastX = GetLastXInLine(Cursor->GetY());
			if (lastX < 0)
				break;
			Cursor->SetX(lastX + 1);
			if (Cursor->GetX() > Gr->Cols - 3) {
				while (Cursor->GetX() != View->GetScrollX()) {
					View->Scroll(1, 0);
				}
				for (int i = 0; i < Gr->Cols - 3; i++) {
					View->Scroll(-1, 0);
				}
			}
		}
		break;
	}
	case SDLK_INSERT: {
		Insert = !Insert;
		break;
	}
	case SDLK_BACKSPACE: {
		TypeBackspace();
		break;
	}
	case SDLK_DELETE: {
		TypeDelete();
		break;
	}
	default: {
		if (key == 32 || (key >= 0 && key <= 255 && isalnum(key))) {
			TypePlainChar(key);
		}
		break;
	}
	}
}

void EditorBase::TypePlainChar(int ch)
{
	if (Insert) {
		auto line = GetObjectsInLine(Cursor->GetY(), Cursor->GetX());
		for (auto& o : line) {
			o->Move(1, 0);
		}
	}

	SceneObject* o = GetObject(Cursor->GetX(), Cursor->GetY());
	if (o == NULL)
		o = new SceneObject();

	o->SetScene(Scrbuf);
	o->SetPosition(Cursor->GetX(), Cursor->GetY());
	o->GetObj()->SetProperty("type", "plain");

	ObjectChar och;

	if (isalpha(ch))
		och.Index = Key::CapsLock() || Key::Shift() ? toupper(ch) : ch;
	else
		och.Index = Key::Shift() ? String::ShiftChar(ch) : ch;

	och.ForeColorIx = ForeColor;
	och.BackColorIx = BackColor;
	o->GetObj()->GetAnimation().Clear();
	o->GetObj()->GetAnimation().AddFrame(och);

	Cursor->Move(1, 0);
}

void EditorBase::TypeBackspace()
{
	if (Cursor->GetX() > 0) {
		Cursor->Move(-1, 0);
		DeleteChar(Cursor->GetX(), Cursor->GetY());
		if (Insert) {
			auto line = GetObjectsInLine(Cursor->GetY(), Cursor->GetX() + 1);
			for (auto& o : line) {
				o->Move(-1, 0);
			}
		}
	}
}

void EditorBase::TypeDelete()
{
	DeleteChar(Cursor->GetX(), Cursor->GetY());
	auto line = GetObjectsInLine(Cursor->GetY(), Cursor->GetX() + 1);
	for (auto& o : line) {
		o->Move(-1, 0);
	}
}

void EditorBase::DeleteChar(int x, int y)
{
	Scrbuf->RemoveObject(GetObject(x, y));
}

void EditorBase::TypeString(std::string str)
{
	int x = Cursor->GetX();
	int y = Cursor->GetY();

	for (auto& ch : str) {
		if (ch == '\n') {
			Crlf();
		}
		else {
			PutPlainChar(ch, x, y);
			Cursor->Move(1, 0);
			x++;
		}
	}
}

void EditorBase::TypeStringCrlf(std::string str)
{
	TypeString(str);
	Crlf();
}

void EditorBase::PutPlainChar(int ch, int x, int y)
{
	SceneObject* o = GetObject(x, y);
	if (o == NULL)
		o = new SceneObject();

	o->SetScene(Scrbuf);
	o->SetPosition(x, y);
	o->GetObj()->SetProperty("type", "plain");

	ObjectChar och;
	och.Index = ch;
	och.ForeColorIx = ForeColor;
	och.BackColorIx = BackColor;
	o->GetObj()->GetAnimation().Clear();
	o->GetObj()->GetAnimation().AddFrame(och);
}

void EditorBase::Crlf()
{
	Cursor->SetX(0);
	Cursor->SetY(Cursor->GetY() + 1);
	View->SetScroll(0, View->GetScrollY());
}

void EditorBase::ClearScreen()
{
	Scrbuf->ClearLayer(0);
	Cursor->SetPosition(0, 0);
	View->SetScroll(0, 0);
}

std::vector<SceneObject*> EditorBase::GetObjectsInLine(int line, int column)
{
	std::vector<SceneObject*> objs;

	for (auto& it : Scrbuf->GetObjects()) {
		SceneObject* o = it.second;
		if (o->GetY() == line && o->GetX() >= column && o->GetLayer() == 0) {
			objs.push_back(o);
		}
	}

	std::sort(objs.begin(), objs.end(), [](const auto& lhs, const auto& rhs) {
		return lhs->GetX() < rhs->GetX();
	});

	return objs;
}

std::vector<SceneObject*> EditorBase::GetObjectsInLine(int line)
{
	return GetObjectsInLine(line, 0);
}

std::string EditorBase::GetStringInLine(int line)
{
	std::string str = "";
	auto objs = GetObjectsInLine(line);
	for (auto& o : objs) {
		str.push_back(GetObjectFrame(o, 0).Index);
	}

	return String::Trim(str);
}

int EditorBase::GetLastXInLine(int line)
{
	int lastX = -1;
	for (auto& it : Scrbuf->GetObjects()) {
		SceneObject* o = it.second;
		if (o->GetY() == line && o->GetX() > lastX && o->GetLayer() == 0) {
			lastX = o->GetX();
		}
	}

	return lastX;
}

int EditorBase::GetLastX()
{
	int lastX = -1;
	for (auto& it : Scrbuf->GetObjects()) {
		SceneObject* o = it.second;
		if (o->GetX() > lastX && o->GetLayer() == 0) {
			lastX = o->GetX();
		}
	}

	return lastX;
}

int EditorBase::GetLastY()
{
	int lastY = -1;
	for (auto& it : Scrbuf->GetObjects()) {
		SceneObject* o = it.second;
		if (o->GetY() > lastY && o->GetLayer() == 0) {
			lastY = o->GetY();
		}
	}

	return lastY;
}

SceneObject* EditorBase::GetObject(int x, int y)
{
	return Scrbuf->GetObjectAt(x, y, CHAR_LAYER);
}

std::string EditorBase::GetObjectType(int x, int y)
{
	SceneObject* o = GetObject(x, y);
	return o->GetObj()->GetPropertyAsString("type");
}

bool EditorBase::HasObject(int x, int y)
{
	return GetObject(x, y) != NULL;
}
