#pragma once

#include "GameObject.h"

#include "sceneManager.h"

// --- �{�^���Ǘ������N���X ---
class ButtonManagerBehavior final : public Behavior
{
public:
	void execute(GameObject* obj, float elapsedTime) override;
	void hit(GameObject* src, GameObject* dst, float elapsedTime) override {};
};



// --- �V�[���ύX�����N���X ---
class SceneTransitionBehavior : public Behavior
{
public:
	void execute(GameObject* obj, float elapsedTime) override = 0;
	void hit(GameObject* src, GameObject* dst, float elapsedTime) override = 0;

	void setScene(Scene& scene);
};


class TitleStartButtonBehavior final : public SceneTransitionBehavior
{
public:
	void execute(GameObject* obj, float elapsedTime);
	void hit(GameObject* src, GameObject* dst, float elapsedTime) {};
};
