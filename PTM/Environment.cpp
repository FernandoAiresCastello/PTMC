#include "Environment.h"

Environment::Environment()
{
	Util::Randomize();
}

Environment::~Environment()
{
	DeleteViews();
	delete Proj;
}

void Environment::DeleteViews()
{
	for (auto it = Views.begin(); it != Views.end(); ++it) {
		delete it->second.Viewport;
	}
	Views.clear();
}

void Environment::InitWindow(int cols, int rows, int width, int height, int fullscreen)
{
	Gr = new Graphics(8 * cols, 8 * rows, width, height, fullscreen);
	Gr->SetWindowTitle(WindowTitle);
	Ui = new UIContext(Gr, 0xffffff, 0x000000);
	Ui->Clear();
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

void Environment::PushToCallStack(int programPtr)
{
	CallStack.push(programPtr);
}

int Environment::PopFromCallStack()
{
	int top = CallStack.top();
	CallStack.pop();
	return top;
}

bool Environment::LoadProject(std::string filename)
{
	if (!File::Exists(filename))
		return false;

	delete Proj;
	Proj = new Project();
	bool ok = Proj->Load(filename);

	if (ok) {
		CurrentMap = NULL;
		DeleteViews();
		Maps.clear();

		Ui->Pal = Proj->GetPalette();
		Ui->Chars = Proj->GetCharset();

		auto maps = Proj->GetMaps();
		for (int i = 0; i < maps.size(); i++) {
			Map* map = maps[i];
			Maps[map->GetName()] = map;
		}
	}
	else {
		Proj = NULL;
	}

	return ok;
}

void Environment::AddMap(Map* map)
{
	if (Proj == NULL) {
		Proj = new Project();
		Proj->SetCharset(Ui->Chars);
		Proj->SetPalette(Ui->Pal);
	}
	Proj->AddMap(map);
	Maps[map->GetName()] = map;
}

Map* Environment::GetMap(std::string& id)
{
	return Maps[id];
}

void Environment::AddView(std::string& id, MapViewport* view)
{
	Views[id] = View(view, true);
}

MapViewport* Environment::GetView(std::string& id)
{
	return Views[id].Viewport;
}

void Environment::EnableView(std::string& id, bool enable)
{
	Views[id].Enabled = enable;
}

void Environment::DrawEnabledViews()
{
	for (auto it = Views.begin(); it != Views.end(); it++) {
		View& view = (*it).second;
		if (view.Enabled) {
			view.Viewport->Draw();
		}
	}
}
