#pragma once

#include "GameObject.h"

class PlayerBehavior : public Behavior
{
public:
	void execute(GameObject* obj, float elapsedTime) override;
	void hit(GameObject* src, GameObject* dst, float elapsedTime) override {};

	void turn(GameObject* obj, Vector2 vec, float speed, float elapsedTime);
};


class PickAxeBehavior : public Behavior
{
public:
	void execute(GameObject* obj, float elapsedTime) override;
	void hit(GameObject* src, GameObject* dst, float elapsedTime) override;
};