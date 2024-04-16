#pragma once

#include <wrl/client.h>

#include <d2d1_1.h>
#pragma comment(lib,"d2d1.lib")

#include <dwrite.h>
#pragma comment(lib,"dwrite.lib")

#include <memory>
#include <string>

#include "spriteBatch.h"


// --- �t�H���g�Ǘ��N���X ---
class FontManager {
private:
	FontManager();
	~FontManager() {}

	FontManager(const FontManager&) = delete;
	FontManager& operator=(const FontManager&) = delete;
	FontManager(FontManager&&) noexcept = delete;
	FontManager& operator=(FontManager&&) noexcept = delete;


public:
	// --- �C���X�^���X���擾 ---
	static FontManager& instance() {
		static FontManager instance;
		return instance;
	}


	// --- �O���錾 ---
	enum class Name;
	enum class Color;


	// --- �A�N�Z�T�֐� ---
	IDWriteTextFormat*		getTextFormat(int index) { return textFormats_.at(index).Get(); }
	ID2D1SolidColorBrush*	getColorBrush(int index) { return colorBrushes_.at(index).Get(); }


	// --- dWrite�֘A ---
	void createTextFormat(const wchar_t* fontName, float fontSize);	// �t�H���g�̍쐬
	void createTextFormat(Name name, float fontSize);

	void createColorBrush(const D2D1_COLOR_F& color);				// �J���[�u���V�̍쐬
	void createColorBrush(D2D1::ColorF color);

	void draw(	//	dWrite���g�p����������̕`��
		int fontIndex,																// �t�H���g�̃C���f�b�N�X
		const wchar_t* str,															// �`�悷�镶����
		const DirectX::XMFLOAT2& position,											// �`��ʒu
		const DirectX::XMFLOAT2& dimension = (DirectX::XMFLOAT2(1920.0f, 1080.0f)),	// �`�敝 ( �����z����Ɖ��s���� )
		int colorIndex = (static_cast<int>(Color::WHITE))							// �J���[�u���V�̃C���f�b�N�X
	);

	void draw(
		Name name,																	// �t�H���g�̖��O
		const wchar_t* str,															// �`�悷�镶����
		const DirectX::XMFLOAT2& position,											// �`��ʒu
		const DirectX::XMFLOAT2& dimension = (DirectX::XMFLOAT2(1920.0f, 1080.0f)),	// �`�敝 ( �����z����Ɖ��s���� )
		Color color = (Color::WHITE)												// �F
	);


	// --- �e�N�X�`���֘A ---
	void loadTexture(const wchar_t* fileName);	//	�e�N�X�`���̓ǂݍ���

	void draw(	//	�e�N�X�`�����g�p����������̕`��
		int fontNo, std::string str,
		const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& scale,
		const DirectX::XMFLOAT4& color
	);


	// --- �t�H���g�̖��O ---
	enum class Name { RAGE };

	inline static const wchar_t* names_[]{
		L"Rage"
	};

	// --- �t�H���g�̐F ---
	enum class Color { WHITE };


private:
	// --- dWrite�֘A ---
	Microsoft::WRL::ComPtr<IDWriteFactory>						dWriteFactory_;	// �t�H���g�̍쐬�p
	std::vector<Microsoft::WRL::ComPtr<IDWriteTextFormat>>		textFormats_;	// �e�L�X�g�t�H�[�}�b�g
	std::vector<Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>>	colorBrushes_;	// �J���[�u���V

	// --- �e�N�X�`���֘A ---
	std::vector<std::unique_ptr<SpriteBatch>> fontTextures_;	// �t�H���g�̃e�N�X�`��
};

