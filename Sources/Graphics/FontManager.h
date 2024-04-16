#pragma once

#include <wrl/client.h>

#include <d2d1_1.h>
#pragma comment(lib,"d2d1.lib")

#include <dwrite.h>
#pragma comment(lib,"dwrite.lib")

#include <memory>
#include <string>

#include "spriteBatch.h"


// --- フォント管理クラス ---
class FontManager {
private:
	FontManager();
	~FontManager() {}

	FontManager(const FontManager&) = delete;
	FontManager& operator=(const FontManager&) = delete;
	FontManager(FontManager&&) noexcept = delete;
	FontManager& operator=(FontManager&&) noexcept = delete;


public:
	// --- インスタンスを取得 ---
	static FontManager& instance() {
		static FontManager instance;
		return instance;
	}


	// --- 前方宣言 ---
	enum class Name;
	enum class Color;


	// --- アクセサ関数 ---
	IDWriteTextFormat*		getTextFormat(int index) { return textFormats_.at(index).Get(); }
	ID2D1SolidColorBrush*	getColorBrush(int index) { return colorBrushes_.at(index).Get(); }


	// --- dWrite関連 ---
	void createTextFormat(const wchar_t* fontName, float fontSize);	// フォントの作成
	void createTextFormat(Name name, float fontSize);

	void createColorBrush(const D2D1_COLOR_F& color);				// カラーブラシの作成
	void createColorBrush(D2D1::ColorF color);

	void draw(	//	dWriteを使用した文字列の描画
		int fontIndex,																// フォントのインデックス
		const wchar_t* str,															// 描画する文字列
		const DirectX::XMFLOAT2& position,											// 描画位置
		const DirectX::XMFLOAT2& dimension = (DirectX::XMFLOAT2(1920.0f, 1080.0f)),	// 描画幅 ( 幅を越えると改行する )
		int colorIndex = (static_cast<int>(Color::WHITE))							// カラーブラシのインデックス
	);

	void draw(
		Name name,																	// フォントの名前
		const wchar_t* str,															// 描画する文字列
		const DirectX::XMFLOAT2& position,											// 描画位置
		const DirectX::XMFLOAT2& dimension = (DirectX::XMFLOAT2(1920.0f, 1080.0f)),	// 描画幅 ( 幅を越えると改行する )
		Color color = (Color::WHITE)												// 色
	);


	// --- テクスチャ関連 ---
	void loadTexture(const wchar_t* fileName);	//	テクスチャの読み込み

	void draw(	//	テクスチャを使用した文字列の描画
		int fontNo, std::string str,
		const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& scale,
		const DirectX::XMFLOAT4& color
	);


	// --- フォントの名前 ---
	enum class Name { RAGE };

	inline static const wchar_t* names_[]{
		L"Rage"
	};

	// --- フォントの色 ---
	enum class Color { WHITE };


private:
	// --- dWrite関連 ---
	Microsoft::WRL::ComPtr<IDWriteFactory>						dWriteFactory_;	// フォントの作成用
	std::vector<Microsoft::WRL::ComPtr<IDWriteTextFormat>>		textFormats_;	// テキストフォーマット
	std::vector<Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>>	colorBrushes_;	// カラーブラシ

	// --- テクスチャ関連 ---
	std::vector<std::unique_ptr<SpriteBatch>> fontTextures_;	// フォントのテクスチャ
};

