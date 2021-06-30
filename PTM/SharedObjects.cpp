#include "SharedObjects.h"

struct {
	Project* Proj;
	std::string WorkingDir;
} SharedObject;

void InitSharedObjects()
{
	SharedObject.Proj = new Project();
	SharedObject.Proj->Load("ptm.tgpro");
	SharedObject.WorkingDir = ".";;
}

void DestroySharedObjects()
{
	delete SharedObject.Proj;
	SharedObject.Proj = NULL;
}

Project* GetSharedProject()
{
	return SharedObject.Proj;
}

std::string GetSharedDirectory()
{
	return SharedObject.WorkingDir;
}

void SetSharedDirectory(std::string path)
{
	SharedObject.WorkingDir = path;
}
