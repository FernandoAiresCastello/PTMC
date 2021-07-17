#include "Datafile.h"

Datafile::Datafile()
{
	Chars = new Charset();
	Pal = new Palette();
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
