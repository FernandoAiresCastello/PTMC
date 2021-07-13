#pragma once
#include <map>
#include <string>
#include <vector>
#include <TBRLGPT.h>
using namespace TBRLGPT;

class GameObject : public Object
{
public:
	GameObject();
	~GameObject();

	int X;
	int Y;
	int Layer;
};
