#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>


class Sprite {

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

	public:
		void render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>,
			const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& scale,
			const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize,
			const DirectX::XMFLOAT2& center,
			const float&, const DirectX::XMFLOAT4&
		) const;


		void render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>,

			const float&, const float&,		//	矩形の左上の座標（スクリーン座標系
			const float&, const float&,		//	矩形のサイズ（スクリーン座標系）
			const float&, const float&,
			const float&, const float&,
			const float&, const float&,
			const float& angle,
			const float& r, const float& g, const float& b, const float& a
		) const;

		void textout(Microsoft::WRL::ComPtr<ID3D11DeviceContext>,
			std::string, const DirectX::XMFLOAT2&,
			const DirectX::XMFLOAT2&, const DirectX::XMFLOAT4&);


		Sprite(ID3D11Device* device, const wchar_t* fileName);
		~Sprite();

		struct Vertex {

			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT4 color;
			DirectX::XMFLOAT2 texcoord;
		};

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
		D3D11_TEXTURE2D_DESC texture2dDesc;
};


static auto rotate = [](float& x, float& y, float cx, float cy, float angle) {

	x -= cx;
	y -= cy;

	float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
	float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
	float tx{ x }, ty{ y };
	x = cos * tx + -sin * ty;
	y = sin * tx + cos * ty;

	x += cx;
	y += cy;
};