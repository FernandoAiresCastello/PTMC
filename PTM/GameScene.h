#pragma once
#include <map>
#include <string>
#include <vector>
#include <TBRLGPT.h>
#include "GameObject.h"
using namespace TBRLGPT;

class GameScene
{
public:
	GameScene();
	~GameScene();

	void AddObject(GameObject* o);

	std::string Id;
	std::vector<GameObject*> Objs;
};
