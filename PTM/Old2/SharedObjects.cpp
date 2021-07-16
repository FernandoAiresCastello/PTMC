#include "SharedObjects.h"

Project* Proj;

void InitSharedObjects()
{
	Proj = new Project();
	Proj->Load("ptm.tgpro");
}

void DestroySharedObjects()
{
	delete Proj;
	Proj = NULL;
}

Project* GetSharedProject()
{
	return Proj;
}

Charset* GetSharedCharset()
{
	return Proj->GetCharset();
}

Palette* GetSharedPalette()
{
	return Proj->GetPalette();
}
