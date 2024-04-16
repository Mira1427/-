#pragma once

#include "GameObject.h"

class TitleEraser final : public Eraser
{
public:
	void execute(GameObject* object) override;
};

class GameEraser final : public Eraser
{
public:
	void execute(GameObject* object) override;
};

class ResultEraser final : public Eraser
{
public:
	void execute(GameObject* object) override;
};

