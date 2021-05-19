#include "Environment.h"

Environment::Environment()
{
	Util::Randomize();
}

Environment::~Environment()
{
	for (auto it = Views.begin(); it != Views.end(); ++it) {
		delete it->second;
	}
	Views.clear();

	delete Proj;
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

void Environment::LoadProject(std::string filename)
{
	delete Proj;
	Proj = new Project();
	Proj->Load(filename);
	Ui->Pal = Proj->GetPalette();
	Ui->Chars = Proj->GetCharset();
}

void Environment::AddMap(Map* map)
{
	Proj->AddMap(map);
	Maps[map->GetId()] = map;
}

Map* Environment::GetMap(std::string& id)
{
	return Maps[id];
}

void Environment::AddView(std::string& id, MapViewport* view)
{
	Views[id] = view;
}

MapViewport* Environment::GetView(std::string& id)
{
	return Views[id];
}
