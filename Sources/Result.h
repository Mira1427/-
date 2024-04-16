#pragma once

#include "scene.h"

#include "Library/Vector.h"

class Result : public Scene {
private:
	Result() {}
	~Result() {}
	Result(const Result&) = delete;
	Result& operator=(const Result&) = delete;
	Result(Result&&) noexcept = delete;
	Result& operator=(Result&&) noexcept = delete;

public:
	static Result& instance() {
		static Result instance;
		return instance;
	}

	void initialize() override;
	void deinitialize() override;
	void update(float elapsedTime) override;
	void render(ID3D11DeviceContext* dc) override;
	void ImGui() override {};

private:
	Vector4 backColor{ 0.4f, 0.4f, 0.4f, 1.0f };
};

