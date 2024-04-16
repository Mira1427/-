#pragma once

#include "GameObject.h"

class SpriteBehavior final : public Behavior
{
public:
	void execute(GameObject* obj, float elapsedTime) override;
	void hit(GameObject* src, GameObject* dst, float elapsedTime) override {};
};

