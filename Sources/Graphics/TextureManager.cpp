#include "TextureManager.h"

#include "../../External/ImGui/imgui.h"

std::list<const char*> TextureManager::fileNames_ =
{
	"./Data/Images/Enemy/Rabbit.png",
};

// ===== �e�N�X�`���Ǘ��N���X =============================================================================================================================

// --- �R���X�g���N�^ ---
TextureManager::TextureManager()
{
	pTextures_.clear();
}


// --- �f�X�g���N�^ ---
TextureManager::~TextureManager()
{
	pTextures_.clear();
}


// --- �ǂݍ��� ---
bool TextureManager::load(ID3D11Device* device, const char* fileName)
{
	pTextures_.emplace_back(std::make_unique<Texture>(device, fileName));
	return true;
}

bool TextureManager::load(ID3D11Device* device)
{
	for (auto& fileName : fileNames_)
	{
		load(device, fileName);
	}

	return false;
}

bool TextureManager::load(ID3D11Device* device, std::list<const char*>& fileNames)
{
	for (auto& fileName : fileNames)
	{
		load(device, fileName);
	}

	return true;
}


// --- �f�o�b�OGui�X�V ---
void TextureManager::updateDebugGui()
{
	if (ImGui::BeginTabItem(u8"�e�N�X�`���֘A"))
	{
		std::vector<const char*> textureNames;

		for (const auto& texture : pTextures_)
			textureNames.emplace_back(texture->fileName_);

		if (ImGui::Combo(u8"�e�N�X�`��", &debugIndex, textureNames.data(), static_cast<int>(textureNames.size())));

		auto& texture = pTextures_.at(static_cast<size_t>(debugIndex));

		ImGui::Spacing();
		ImGui::Text(u8"�T�C�Y :%f :%f", texture->size_.x, texture->size_.y);

		float rate = texture->size_.x / texture->size_.y;
		Vector2 size = { texture->size_.x * (300.0f / texture->size_.x) * rate, texture->size_.y * (300.0f / texture->size_.y) };
		ImGui::Image(texture->texture_.Get(), ImVec2(size.x, size.y));

		ImGui::EndTabItem();
	}
}
