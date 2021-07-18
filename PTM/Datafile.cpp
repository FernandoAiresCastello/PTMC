#include "Datafile.h"

Datafile::Datafile()
{
	Chars = new Charset();
	Chars->InitDefaultCharset();
	Pal = new Palette();
	Pal->InitDefaultColors();
	Prog = new Program();
}

Datafile::~Datafile()
{
	delete Chars;
	delete Pal;
	delete Prog;

	for (int i = 0; i < Scenes.size(); i++) {
		delete Scenes[i];
		Scenes[i] = NULL;
	}
	Scenes.clear();

	for (int i = 0; i < ObjTemplates.size(); i++) {
		delete ObjTemplates[i];
		ObjTemplates[i] = NULL;
	}
	ObjTemplates.clear();
}

Program* Datafile::GetProgram()
{
	return Prog;
}

void Datafile::SetProgram(Program* prog)
{
	Prog = prog;
}

Charset* Datafile::GetCharset()
{
	return Chars;
}

Palette* Datafile::GetPalette()
{
	return Pal;
}

std::vector<SceneObject*>& Datafile::GetTemplates()
{
	return ObjTemplates;
}

void Datafile::SetTemplate(std::string id, SceneObject* o)
{
	SceneObject* templ = GetTemplate(id);
	if (templ == NULL) {
		templ = new SceneObject();
		ObjTemplates.push_back(templ);
	}

	templ->SetEqual(o);
	templ->GetObj()->SetProperty("temp_id", id);
}

SceneObject* Datafile::GetTemplate(std::string id)
{
	for (auto& templ : ObjTemplates) {
		if (templ->GetObj()->GetPropertyAsString("temp_id") == id)
			return templ;
	}

	return NULL;
}

void Datafile::DeleteObjTemplate(std::string id)
{
	int tempIndex = -1;
	for (int i = 0; i < ObjTemplates.size(); i++) {
		SceneObject* o = ObjTemplates[i];
		if (o->GetObj()->GetPropertyAsString("temp_id") == id) {
			tempIndex = i;
			break;
		}
	}

	if (tempIndex >= 0)
		ObjTemplates.erase(ObjTemplates.begin() + tempIndex);
}
