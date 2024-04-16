#pragma once

#include "GameObject.h"

#include "sceneManager.h"

// --- ボタン管理処理クラス ---
class ButtonManagerBehavior final : public Behavior
{
public:
	void execute(GameObject* obj, float elapsedTime) override;
	void hit(GameObject* src, GameObject* dst, float elapsedTime) override {};
};



// --- シーン変更処理クラス ---
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
