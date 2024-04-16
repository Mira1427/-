#include "SceneEraser.h"

#include "sceneManager.h"
#include "title.h"
#include "game.h"
#include "Result.h"

void TitleEraser::execute(GameObject* object)
{
	if (SceneManager::instance().scene != &Title::instance())
		object->destroy();
}

void GameEraser::execute(GameObject* object)
{
	if (SceneManager::instance().scene != &Game::instance())
		object->destroy();
}

void ResultEraser::execute(GameObject* object)
{
	if (SceneManager::instance().scene != &Result::instance())
		object->destroy();
}
