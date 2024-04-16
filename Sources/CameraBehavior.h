#pragma once

#include "GameObject.h"


class BaseCameraBehavior : public Behavior
{
public:
	virtual void execute(GameObject* obj, float elapsedTime) override;
	void hit(GameObject* src, GameObject* dst, float elapsedTime) override {};
};

class DebugCameraBehavior : public BaseCameraBehavior
{
public:
	void execute(GameObject* obj, float elapsedTime) override;

private:
	void applyMayaControl(GameObject* obj, float elapsedTime);
};

