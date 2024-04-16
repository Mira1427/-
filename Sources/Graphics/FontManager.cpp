#include "FontManager.h"

#include "../misc.h"
#include "Graphics.h"


// --- コンストラクタ ---
FontManager::FontManager() {

	HRESULT hr{ S_OK };

	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(dWriteFactory_.GetAddressOf())
	);

	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


// ===== dWrite関連の処理 ===================================================================================================================

// --- フォントの作成 ---
// --- フォントの名前は 設定 / 個人用設定 / フォントから確認可能 ---
void FontManager::createTextFormat(const wchar_t* fontName, float fontSize) {

	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat;

	hr = dWriteFactory_.Get()->CreateTextFormat(	// フォーマットを作成
		fontName,
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		L"",
		textFormat.ReleaseAndGetAddressOf()
	);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = textFormat.Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);	// アラインメントの設定
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	textFormats_.emplace_back(textFormat);	// 配列に追加
}

void FontManager::createTextFormat(Name name, float fontSize) {

	createTextFormat(names_[static_cast<int>(name)], fontSize);
}


// --- カラーブラシの作成 ---
void FontManager::createColorBrush(const D2D1_COLOR_F& color) {

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> colorBrush;

	Graphics::instance().getD2D1DeviceContext()->CreateSolidColorBrush(		// ブラシの作成
		color,
		colorBrush.GetAddressOf()
	);

	colorBrushes_.emplace_back(colorBrush);		// 配列に追加
}

void FontManager::createColorBrush(D2D1::ColorF color) {

	createColorBrush(static_cast<D2D1_COLOR_F>(color));
}


// --- dWriteを使用した文字列の描画 ---
void FontManager::draw(
	int fontIndex, const wchar_t* str,
	const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& dimension,
	int colorIndex) {

	HRESULT hr{ S_OK };

	Graphics::instance().getD2D1DeviceContext()->BeginDraw();		// 描画開始

	Graphics::instance().getD2D1DeviceContext()->DrawTextW(			// 描画
		str,
		wcslen(str),
		textFormats_.at(fontIndex).Get(),
		D2D1::RectF(
			position.x,  position.y,
			dimension.x, dimension.y
		),
		colorBrushes_.at(colorIndex).Get(),
		D2D1_DRAW_TEXT_OPTIONS_NONE
	);

	hr = Graphics::instance().getD2D1DeviceContext()->EndDraw();	// 描画終了

	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void FontManager::draw(
	Name name, const wchar_t* str,
	const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& dimension,
	Color color) {

	draw(static_cast<int>(name), str, position, dimension, static_cast<int>(color));
}


// ===== テクスチャ関連の処理 ==============================================================================================================

// --- テクスチャの読み込み ---
void FontManager::loadTexture(const wchar_t* fileName) {

	fontTextures_.emplace_back(
		std::make_unique<SpriteBatch>(
			Graphics::instance().getDevice(),	// デバイス
			fileName,							// ファイル名
			100U								// 最大インスタンス数
		)
	);
}


// --- テクスチャを使用した文字列の描画 ---
void FontManager::draw(
	int fontNo, std::string str, 
	const DirectX::XMFLOAT2& position, 
	const DirectX::XMFLOAT2& scale, 
	const DirectX::XMFLOAT4& color) {

	float sw = static_cast<float>(fontTextures_.at(fontNo)->texture2dDesc.Width / 16);
	float sh = static_cast<float>(fontTextures_.at(fontNo)->texture2dDesc.Height / 16);
	float carriage = 0;

	fontTextures_.at(fontNo)->begin(Graphics::instance().getDeviceContext());	// 描画開始

	for (const char c : str) {	// 描画

		fontTextures_.at(fontNo)->render(Graphics::instance().getDeviceContext(),
			DirectX::XMFLOAT2(position.x + carriage, position.y),
			scale, DirectX::XMFLOAT2(sw * (c & 0x0F), sh * (c >> 4)),
			DirectX::XMFLOAT2(sw, sh), DirectX::XMFLOAT2(0, 0), 0,
			color
		);

		carriage += sw;
	}

	fontTextures_.at(fontNo)->end(Graphics::instance().getDeviceContext());	// 描画終了
}
