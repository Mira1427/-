#pragma once

#include "GameObject.h"

class ModelBehavior final : public Behavior
{
	void execute(GameObject* obj, float elapsedTime) override;
	void hit(GameObject* src, GameObject* dst, float elapsedTime) override {};
};

