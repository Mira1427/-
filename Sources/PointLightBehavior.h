#pragma once

#include "GameObject.h"

class PointLightBehavior final : public Behavior
{
public:
	void execute(GameObject* obj, float elapsedTime) override;
	void hit(GameObject* src, GameObject* dst, float elapsedTime) override {}
};