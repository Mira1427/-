#pragma once

#include "scene.h"

#include "./Graphics/SkyMap.h"
#include "./Graphics/staticMesh.h"

// --- ゲームクラス ---
class Game : public Scene {
private:
	Game() {}
	~Game() {}

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	Game(Game&&) noexcept = delete;
	Game& operator=(Game&&) noexcept = delete;

public:
	// --- インスタンスを取得 ---
	static Game& instance() {
		static Game instance;
		return instance;
	}

	void initialize() override;
	void deinitialize() override;
	void update(float elapsedTime) override;
	void render(ID3D11DeviceContext* dc) override;
	void ImGui() override;

	void addBlock(const Vector3& position);

	std::unique_ptr<SkyMap> skyMap_;
	std::shared_ptr<StaticMesh> cube_;
};

