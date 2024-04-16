#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>
#include <memory>


class Primitive {

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

public:
	void render(ID3D11DeviceContext*,
		const DirectX::XMFLOAT2& position,
		const DirectX::XMFLOAT2& size, const DirectX::XMFLOAT2& center,
		const float&, const DirectX::XMFLOAT4&
	) const;


	void render(ID3D11DeviceContext*,
										
		const float&, const float&,		//	矩形の左上の座標（スクリーン座標系
		const float&, const float&,		//	矩形のサイズ（スクリーン座標系）
		const float&, const float&,
		const float& angle,
		const float& r, const float& g, const float& b, const float& a
	) const;


	Primitive(ID3D11Device* device);
	~Primitive();

	struct Vertex {

		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3D11_TEXTURE2D_DESC texture2dDesc;
};


static auto rotate_c = [](float& x, float& y, float cx, float cy, float angle) {

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