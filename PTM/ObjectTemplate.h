#pragma once
#include <string>
#include <TileGameLib.h>
using namespace TileGameLib;

class ObjectTemplate
{
public:
	std::string Id;
	TObject* Obj;

	ObjectTemplate();
	ObjectTemplate(const ObjectTemplate& other);
	~ObjectTemplate();

private:
};
