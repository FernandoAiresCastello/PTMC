#include <algorithm>
#include "CommandShell.h"

#define CHAR_LAYER 0
#define ERR_SYNTAX_ERROR "Syntax error"

CommandShell::CommandShell(Graphics* gr, Datafile* data)
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

CommandShell::~CommandShell()
{
	delete View;
	delete Scrbuf;
}

void CommandShell::Run()
{
	Running = true;

	PrintIntro();

	while (Running) {
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

void CommandShell::Draw()
{
	Gr->Clear(Data->GetPalette()->Get(BorderColor)->ToInteger());
	View->Draw();
}

void CommandShell::SetForeColor(int color)
{
	ForeColor = color;
	for (auto& it : Scrbuf->GetObjects()) {
		Object* o = it.second->GetObj();
		if (o->HasPropertyValue("type", "plain")) {
			GetObjectFrame(o, 0).ForeColorIx = color;
		}
	}
}

void CommandShell::SetBackColor(int color)
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

void CommandShell::SetBorderColor(int color)
{
	BorderColor = color;
}

void CommandShell::UpdateCursor()
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

ObjectChar& CommandShell::GetCursorFrame(int frame)
{
	return GetObjectFrame(Cursor, frame);
}

ObjectChar& CommandShell::GetObjectFrame(SceneObject* o, int frame)
{
	return o->GetObj()->GetAnimation().GetFrame(frame);
}

ObjectChar& CommandShell::GetObjectFrame(Object* o, int frame)
{
	return o->GetAnimation().GetFrame(frame);
}

void CommandShell::OnKeyPress(SDL_Keycode key)
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

void CommandShell::TypePlainChar(int ch)
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

void CommandShell::TypeBackspace()
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

void CommandShell::TypeDelete()
{
	DeleteChar(Cursor->GetX(), Cursor->GetY());
	auto line = GetObjectsInLine(Cursor->GetY(), Cursor->GetX() + 1);
	for (auto& o : line) {
		o->Move(-1, 0);
	}
}

void CommandShell::DeleteChar(int x, int y)
{
	Scrbuf->RemoveObject(GetObject(x, y));
}

void CommandShell::TypeString(std::string str)
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

void CommandShell::TypeStringCrlf(std::string str)
{
	TypeString(str);
	Crlf();
}

void CommandShell::PutPlainChar(int ch, int x, int y)
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

void CommandShell::Crlf()
{
	Cursor->SetX(0);
	Cursor->SetY(Cursor->GetY() + 1);
}

void CommandShell::ClearScreen()
{
	Scrbuf->ClearLayer(0);
	Cursor->SetPosition(0, 0);
	View->SetScroll(0, 0);
}

std::vector<SceneObject*> CommandShell::GetObjectsInLine(int line, int column)
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

std::vector<SceneObject*> CommandShell::GetObjectsInLine(int line)
{
	return GetObjectsInLine(line, 0);
}

std::string CommandShell::GetStringInLine(int line)
{
	std::string str = "";
	auto objs = GetObjectsInLine(line);
	for (auto& o : objs) {
		str.push_back(GetObjectFrame(o, 0).Index);
	}

	return String::Trim(str);
}

int CommandShell::GetLastXInLine(int line)
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

SceneObject* CommandShell::GetObject(int x, int y)
{
	return Scrbuf->GetObjectAt(x, y, CHAR_LAYER);
}

std::string CommandShell::GetObjectType(int x, int y)
{
	SceneObject* o = GetObject(x, y);
	return o->GetObj()->GetPropertyAsString("type");
}

bool CommandShell::HasObject(int x, int y)
{
	return GetObject(x, y) != NULL;
}

void CommandShell::PrintIntro()
{
	TypeStringCrlf("PTM 1.0");
	Ok();
}

void CommandShell::Ok()
{
	TypeStringCrlf("Ok");
}

void CommandShell::TypeEnter()
{
	std::string type = "";

	if (HasObject(Cursor->GetX(), Cursor->GetY()))
		type = GetObjectType(Cursor->GetX(), Cursor->GetY());

	if (type == "" || type == "plain") {
		Crlf();
		std::string line = GetStringInLine(Cursor->GetY() - 1);
		if (!line.empty()) {
			InterpretLine(line);
		}
	}
	else {
		// todo
	}
}

void CommandShell::InterpretLine(std::string line)
{
	int ixFirstSpace = line.find(' ');
	std::string cmd = String::ToLower(ixFirstSpace >= 0 ? line.substr(0, ixFirstSpace) : line);
	std::string rawParams = ixFirstSpace >= 0 ? line.substr(ixFirstSpace) : "";
	auto params = String::Split(rawParams, ',');
	std::string error = "";

	if (cmd == "exit") {
		Running = false;
		return;
	}
	else if (cmd == "fgcolor") {
		if (params.size() != 1) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			SetForeColor(String::ToInt(params[0]));
		}
	}
	else if (cmd == "bgcolor") {
		if (params.size() != 1) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			SetBackColor(String::ToInt(params[0]));
		}
	}
	else if (cmd == "bdcolor") {
		if (params.size() != 1) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			SetBorderColor(String::ToInt(params[0]));
		}
	}
	else {
		error = ERR_SYNTAX_ERROR;
	}

	if (!error.empty())
		TypeStringCrlf(error);

	Ok();
}
