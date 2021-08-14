#include "ObjectTemplate.h"

ObjectTemplate::ObjectTemplate()
{
	Obj = new TObject();
}

ObjectTemplate::ObjectTemplate(const ObjectTemplate& other)
{
	Id = other.Id;
	Obj = other.Obj;
}

ObjectTemplate::~ObjectTemplate()
{
	delete Obj;
}
