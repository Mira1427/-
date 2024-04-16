#include "FontManager.h"

#include "../misc.h"
#include "Graphics.h"


// --- �R���X�g���N�^ ---
FontManager::FontManager() {

	HRESULT hr{ S_OK };

	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(dWriteFactory_.GetAddressOf())
	);

	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


// ===== dWrite�֘A�̏��� ===================================================================================================================

// --- �t�H���g�̍쐬 ---
// --- �t�H���g�̖��O�� �ݒ� / �l�p�ݒ� / �t�H���g����m�F�\ ---
void FontManager::createTextFormat(const wchar_t* fontName, float fontSize) {

	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<IDWriteTextFormat> textFormat;

	hr = dWriteFactory_.Get()->CreateTextFormat(	// �t�H�[�}�b�g���쐬
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

	hr = textFormat.Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);	// �A���C�������g�̐ݒ�
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	textFormats_.emplace_back(textFormat);	// �z��ɒǉ�
}

void FontManager::createTextFormat(Name name, float fontSize) {

	createTextFormat(names_[static_cast<int>(name)], fontSize);
}


// --- �J���[�u���V�̍쐬 ---
void FontManager::createColorBrush(const D2D1_COLOR_F& color) {

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> colorBrush;

	Graphics::instance().getD2D1DeviceContext()->CreateSolidColorBrush(		// �u���V�̍쐬
		color,
		colorBrush.GetAddressOf()
	);

	colorBrushes_.emplace_back(colorBrush);		// �z��ɒǉ�
}

void FontManager::createColorBrush(D2D1::ColorF color) {

	createColorBrush(static_cast<D2D1_COLOR_F>(color));
}


// --- dWrite���g�p����������̕`�� ---
void FontManager::draw(
	int fontIndex, const wchar_t* str,
	const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& dimension,
	int colorIndex) {

	HRESULT hr{ S_OK };

	Graphics::instance().getD2D1DeviceContext()->BeginDraw();		// �`��J�n

	Graphics::instance().getD2D1DeviceContext()->DrawTextW(			// �`��
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

	hr = Graphics::instance().getD2D1DeviceContext()->EndDraw();	// �`��I��

	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void FontManager::draw(
	Name name, const wchar_t* str,
	const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& dimension,
	Color color) {

	draw(static_cast<int>(name), str, position, dimension, static_cast<int>(color));
}


// ===== �e�N�X�`���֘A�̏��� ==============================================================================================================

// --- �e�N�X�`���̓ǂݍ��� ---
void FontManager::loadTexture(const wchar_t* fileName) {

	fontTextures_.emplace_back(
		std::make_unique<SpriteBatch>(
			Graphics::instance().getDevice(),	// �f�o�C�X
			fileName,							// �t�@�C����
			100U								// �ő�C���X�^���X��
		)
	);
}


// --- �e�N�X�`�����g�p����������̕`�� ---
void FontManager::draw(
	int fontNo, std::string str, 
	const DirectX::XMFLOAT2& position, 
	const DirectX::XMFLOAT2& scale, 
	const DirectX::XMFLOAT4& color) {

	float sw = static_cast<float>(fontTextures_.at(fontNo)->texture2dDesc.Width / 16);
	float sh = static_cast<float>(fontTextures_.at(fontNo)->texture2dDesc.Height / 16);
	float carriage = 0;

	fontTextures_.at(fontNo)->begin(Graphics::instance().getDeviceContext());	// �`��J�n

	for (const char c : str) {	// �`��

		fontTextures_.at(fontNo)->render(Graphics::instance().getDeviceContext(),
			DirectX::XMFLOAT2(position.x + carriage, position.y),
			scale, DirectX::XMFLOAT2(sw * (c & 0x0F), sh * (c >> 4)),
			DirectX::XMFLOAT2(sw, sh), DirectX::XMFLOAT2(0, 0), 0,
			color
		);

		carriage += sw;
	}

	fontTextures_.at(fontNo)->end(Graphics::instance().getDeviceContext());	// �`��I��
}
