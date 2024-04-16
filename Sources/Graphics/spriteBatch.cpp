#include "spriteBatch.h"

#include <sstream>
#include <WICTextureLoader.h>

#include "../misc.h"
#include "Graphics.h"


SpriteBatch::SpriteBatch(ID3D11Device* device, const wchar_t* fileName, size_t maxSprites)
	: maxVertices(maxSprites * 6) {

	//	頂点情報のセット
	Vertex vertices[]{

		{ { -1.0, +1.0, 0 }, {1, 1, 1, 1}, {0, 0} },	//	多分左上
		{ { +1.0, +1.0, 0 }, {1, 1, 1, 1}, {1, 0} },	//	多分右上
		{ { -1.0, -1.0, 0 }, {0.5, 0.5, 0.5, 1}, {0, 1} },	//	多分左下
		{ { +1.0, -1.0, 0 }, {0.5, 0.5, 0.5, 1}, {1, 1} }		//	多分右下
	};

	HRESULT hr{ S_OK };

	//	頂点バッファオブジェクトの生成
	{
		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.ByteWidth = sizeof(Vertex) * maxVertices;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		hr = device->CreateBuffer(&bufferDesc, NULL, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


	//	入力レイアウトオブジェクトの生成
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]{

		{"POSITION",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",				0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",			0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
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


void SpriteBatch::render(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext,
	const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT2& scale,
	const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize,
	const DirectX::XMFLOAT2& center,
	const float& angle, const DirectX::XMFLOAT4& color) {

	//	スクリーン（ビューポート）のサイズを取得する
	D3D11_VIEWPORT viewport{};									//	03
	UINT num_viewport{ 1 };
	immediateContext->RSGetViewports(&num_viewport, &viewport);

	if (scale.x == 0.0f || scale.y == 0.0f) return;

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

	//	RIght_Bottom
	float x3{ position.x + texSize.x * scale.x - center.x * scale.x };
	float y3{ position.y + texSize.y * scale.y - center.y * scale.y };

	//	回転の中心を矩形の中心点にした場合
	float cx = position.x - center.x + texSize.x * 0.5f;
	float cy = position.y - center.y + texSize.y * 0.5f;
	rotate_b(x0, y0, cx, cy, angle);
	rotate_b(x1, y1, cx, cy, angle);
	rotate_b(x2, y2, cx, cy, angle);
	rotate_b(x3, y3, cx, cy, angle);


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
	float u0{ texPos.x / texture2dDesc.Width };
	float v0{ texPos.y / texture2dDesc.Height };
	float u1{ (texPos.x + texSize.x) / texture2dDesc.Width };
	float v1{ (texPos.y + texSize.y) / texture2dDesc.Height };

	vertices.push_back({ {x0, y0, 0}, color, {u0, v0} });
	vertices.push_back({ {x1, y1, 0}, color, {u1, v0} });
	vertices.push_back({ {x2, y2, 0}, color, {u0, v1} });
	vertices.push_back({ {x2, y2, 0}, color, {u0, v1} });
	vertices.push_back({ {x1, y1, 0}, color, {u1, v0} });
	vertices.push_back({ {x3, y3, 0}, color, {u1, v1} });
}



void SpriteBatch::render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext,
	const float& dx, const float& dy, const float& sw, const float& sh,
	const float& tx, const float& ty, const float& tw, const float& th,
	const float& cx, const float& cy, const float& angle,
	const float& r, const float& g, const float& b, const float& a) {

	return render(immediateContext, DirectX::XMFLOAT2(dx, dy), DirectX::XMFLOAT2(sw, sh),
		DirectX::XMFLOAT2(tx, ty), DirectX::XMFLOAT2(tw, th), DirectX::XMFLOAT2(cx, cy), angle, DirectX::XMFLOAT4(r, g, b, a));
}


SpriteBatch::~SpriteBatch() {
}


void SpriteBatch::begin(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext) {

	vertices.clear();
	immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);
	immediateContext->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
}


void SpriteBatch::end(Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext) {

	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedBuffer{};

	hr = immediateContext->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	size_t vertexcount = vertices.size();
	_ASSERT_EXPR(maxVertices >= vertexcount, "Buffer overflow");
	Vertex* data{ reinterpret_cast<Vertex*>(mappedBuffer.pData) };

	if (data != nullptr) {

		const Vertex* p = vertices.data();
		memcpy_s(data, maxVertices * sizeof(Vertex), p, vertexcount * sizeof(Vertex));
	}

	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->IASetInputLayout(inputLayout.Get());

	immediateContext->Unmap(vertexBuffer.Get(), 0);
	immediateContext->Draw(static_cast<UINT>(vertexcount), 0);
}
