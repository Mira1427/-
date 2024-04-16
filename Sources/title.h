#pragma once

#include "scene.h"

#include "Graphics/SkyMap.h"

#include "GameObject.h"

class Title : public Scene {
private:
	Title() {}
	~Title() {}
	Title(const Title&) = delete;
	Title& operator=(const Title&) = delete;
	Title(Title&&) noexcept = delete;
	Title& operator=(Title&&) noexcept = delete;

public:
	static Title& instance() {
		static Title instance;
		return instance;
	}

	void initialize() override;
	void deinitialize() override;
	void update(float) override;
	void render(ID3D11DeviceContext* dc) override;
	void ImGui() override;

	std::unique_ptr<SkyMap> skyMap_;
};

