#include "GameScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	for (int i = 0; i < Objs.size(); i++) {
		delete Objs[i];
		Objs[i] = NULL;
	}

	Objs.clear();
}

void GameScene::AddObject(GameObject* o)
{
	Objs.push_back(o);
}
