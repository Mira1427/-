#pragma once

#include "scene.h"

class SceneManager {
public:
	static SceneManager& instance() { 

		static SceneManager instance_;
		return instance_;
	}

	Scene* scene;
	Scene* nextScene;
};