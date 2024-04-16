#pragma once

#include <vector>

#include "Graphics.h"


// ===== テクスチャ管理クラス =============================================================================================================================
class TextureManager
{
private:
	TextureManager();
	~TextureManager();
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;
	TextureManager(TextureManager&&)noexcept = delete;
	TextureManager& operator=(TextureManager&&)noexcept = delete;

public:
	// --- インスタンスの取得 ---
	static TextureManager& instance()
	{
		static TextureManager instance;
		return instance;
	}

	Texture* getTexture(int index) { return &*pTextures_.at(static_cast<size_t>(index)).get(); }
	std::vector<std::unique_ptr<Texture>>& getTextureList() { return pTextures_; }

	bool load(ID3D11Device* device, const char* fileName);
	bool load(ID3D11Device* device);
	bool load(ID3D11Device* device, std::list<const char*>& fileNames);

	void updateDebugGui();

	static std::list<const char*> fileNames_;

private:
	std::vector<std::unique_ptr<Texture>> pTextures_;

	int debugIndex;
};

