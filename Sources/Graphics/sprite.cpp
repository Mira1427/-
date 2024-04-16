#include "sprite.h"

#include <sstream>
#include <WICTextureLoader.h>

#include "../misc.h"
#include "TextureTool.h"
#include "Graphics.h"


Sprite::Sprite(ID3D11Device* device, const wchar_t* fileName) {

	//	頂点情報のセット
	Vertex vertices[]{

		{ { -1.0, +1.0, 0 }, {1, 1, 1, 1}, {0, 0} },
		{ { +1.0, +1.0, 0 }, {1, 1, 1, 1}, {1, 0} },
		{ { -1.0, -1.0, 0 }, {0.5, 0.5, 0.5, 1}, {0, 1} },
		{ { +1.0, -1.0, 0 }, {0.5, 0.5, 0.5, 1}, {1, 1} }
	};

	HRESULT hr{ S_OK };

	//	頂点バッファオブジェクトの生成
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pSysMem = vertices;
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//	入力レイアウトオブジェクトの生成
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	

	//	頂点シェーダーオブジェクトの生成
	Graphics::createVSAndILFromCSO(device, "./Data/Shaders/spriteVS.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(),
		inputElementDesc, ARRAYSIZE(inputElementDesc));

	//	ピクセルシェーダーオブジェクトの生成
	Graphics::createPSFromCSO(device, "./Data/Shaders/spritePS.cso", pixelShader.GetAddressOf());


	//	画像ファイルとシェーダーリソースビューオブジェクトの生成
	Microsoft::WRL::ComPtr<ID3D11Resource> resource{};
	hr = DirectX::CreateWICTextureFromFile(device, fileName, resource.GetAddressOf(), shaderResourceView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//	テクスチャ情報の取得
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d{};
	hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	texture2d->GetDesc(&texture2dDesc);
}


void Sprite::render(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext,
	const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& scale,
	const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize,
	const DirectX::XMFLOAT2& center,
	const float& angle, const DirectX::XMFLOAT4& color) const  {

	if (scale.x == 0.0f || scale.y == 0.0f) return;

	//	スクリーン（ビューポート）のサイズを取得する
	D3D11_VIEWPORT viewport{};
	UINT numViewport{ 1 };
	immediateContext->RSGetViewports(&numViewport, &viewport);


	//	renderメンバ関数の引数から、矩形の各頂点の位置を計算する
	//	(x0, y0) *----* (x1, y1)
	//			 |   /|
	//			 |  / |
	//			 | /  |
	//			 |/   |
	//	(x2, y2) *----* (x3, y3)

	//	Left_Top
	float x0{ position.x - center.x * scale.x };
	float y0{ position.y - center.y * scale.y };

	//	Right_Top
	float x1{ position.x + texSize.x * scale.x - center.x * scale.x };
	float y1{ position.y - center.y * scale.y };

	//	Left_Bottom
	float x2{ position.x - center.x * scale.x };
	float y2{ position.y + texSize.y * scale.y - center.y * scale.y };

	//	Right_Bottom
	float x3{ position.x + texSize.x * scale.x - center.x * scale.x };
	float y3{ position.y + texSize.y * scale.y - center.y * scale.y };


	//	回転の中心を矩形の中心点にした場合
	float cx = position.x - center.x + texSize.x * 0.5f;
	float cy = position.y - center.y + texSize.y * 0.5f;
	rotate(x0, y0, cx, cy, angle);
	rotate(x1, y1, cx, cy, angle);
	rotate(x2, y2, cx, cy, angle);
	rotate(x3, y3, cx, cy, angle);


	//	スクリーン座標系からNDCへの変換を行う
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;

	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;

	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;

	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;


	//	テクセル座標系からUV座標系への変換を行う
	//tx0 = tx0 / 1960;
	//ty0 = ty0 / 1200;

	//tx1 = tx1 / 1960;
	//ty1 = ty1 / 1200;

	//tx2 = tx2 / 1960;
	//ty2 = ty2 / 1200;

	//tx3 = tx3 / 1960;
	//ty3 = ty3 / 1200;

	float u0{ texPos.x / texture2dDesc.Width };
	float v0{ texPos.y / texture2dDesc.Height };
	float u1{ (texPos.x + texSize.x) / texture2dDesc.Width };
	float v1{ (texPos.y + texSize.y) / texture2dDesc.Height };


	//	計算結果で頂点バッファオブジェクトを更新する
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = immediateContext->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Vertex* vertices{ reinterpret_cast<Vertex*>(mappedSubresource.pData) };
	
	if (vertices != nullptr) {

		vertices[0].position = { x0, y0, 0 };
		vertices[1].position = { x1, y1, 0 };
		vertices[2].position = { x2, y2, 0 };
		vertices[3].position = { x3, y3, 0 };

		//vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { 1, 1, 1, 1 };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = color;

		vertices[0].texcoord = { u0, v0 };
		vertices[1].texcoord = { u1, v0 };
		vertices[2].texcoord = { u0, v1 };
		vertices[3].texcoord = { u1, v1 };
	}

	immediateContext->Unmap(vertexBuffer.Get(), 0);

	//	シェーダーリソースのバインド
	immediateContext->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

	//	頂点バッファーのバインド
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);


	//	プリミティブタイプおよびデータの順序に関する情報のバインド
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	//	入力レイアウトオブジェクトのバインド
	immediateContext->IASetInputLayout(inputLayout.Get());

	
	//	シェーダーのバインド
	immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);


	//	プリミティブの描画
	immediateContext->Draw(4, 0);
}


void Sprite::render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext,
	const float& dx, const float& dy, const float& sw, const float& sh,
	const float& tx, const float& ty, const float& tw, const float& th,
	const float& cx, const float& cy, const float& angle,
	const float& r, const float& g, const float& b, const float& a) const {

	return render(immediateContext, DirectX::XMFLOAT2(dx, dy), DirectX::XMFLOAT2(sw, sh),
		DirectX::XMFLOAT2(tx, ty), DirectX::XMFLOAT2(tw, th), DirectX::XMFLOAT2(cx, cy), angle, DirectX::XMFLOAT4(r, g, b, a));
}


void Sprite::textout(Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext,
	std::string s, const DirectX::XMFLOAT2& position,
	const DirectX::XMFLOAT2& size, const DirectX::XMFLOAT4& color) {

	float sw = static_cast<float>(texture2dDesc.Width / 16);
	float sh = static_cast<float>(texture2dDesc.Height / 16);
	float carriage = 0;

	for (const char c : s) {

		render(immediateContext,
			DirectX::XMFLOAT2(position.x + carriage, position.y),
			size, DirectX::XMFLOAT2(sw * (c & 0x0F), sh * (c >> 4)),
			DirectX::XMFLOAT2(sw, sh), DirectX::XMFLOAT2(0, 0), 0, color);
		//carriage += size.x;
		carriage += sw;
	}
}


Sprite::~Sprite() {
}