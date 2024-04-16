#include "geometricPrimitive.h"
#include "../misc.h"
#include "Graphics.h"

//	コンストラクタ
GeometricPrimitive::GeometricPrimitive(ID3D11Device* device) {

	//	サイズ1.0の正立方体（重心が原点）
	//	頂点8 × 頂点の法線3 = 24
	Vertex vertices[24]{};

	Vertex tempVertices[8]{
		{{-0.5f, 0.5f, -0.5f}},		//	手前左上
		{{0.5f, 0.5f, -0.5f}},		//	手前右上
		{{ -0.5f, -0.5f, -0.5f}},	//	手前左下
		{{0.5f, -0.5f, -0.5f}},		//	手前右下
		{{-0.5f, 0.5f, 0.5f}},		//	奥左上
		{{0.5f, 0.5f, 0.5f}},		//	奥右上
		{{-0.5f, -0.5f, 0.5f}},		//	奥左下
		{{0.5f, -0.5f, 0.5f}},		//	奥右下
	};

	//	手前
	vertices[0].position = tempVertices[0].position;	//	手前左上
	vertices[1].position = tempVertices[1].position;	//	手前右上
	vertices[2].position = tempVertices[2].position;	//	手前左下
	vertices[3].position = tempVertices[3].position;	//	手前右下
	vertices[0].normal = vertices[1].normal = vertices[2].normal = vertices[3].normal = { 0, 0, -1 };

	//	上
	vertices[4].position = tempVertices[4].position;	//	奥左上
	vertices[5].position = tempVertices[5].position;	//	奥右上
	vertices[6].position = tempVertices[0].position;	//	手前左上
	vertices[7].position = tempVertices[1].position;	//	手前右上
	vertices[4].normal = vertices[5].normal = vertices[6].normal = vertices[7].normal = { 0, 1, 0 };

	//	右
	vertices[8].position = tempVertices[1].position;	//	手前右上
	vertices[9].position = tempVertices[5].position;	//	奥右上
	vertices[10].position = tempVertices[3].position;	//	手前右下
	vertices[11].position = tempVertices[7].position;	//	奥右下
	vertices[8].normal = vertices[9].normal = vertices[10].normal = vertices[11].normal = { 1, 0, 0 };

	//	左
	vertices[12].position = tempVertices[4].position;	//	奥左上
	vertices[13].position = tempVertices[0].position;	//	手前左上
	vertices[14].position = tempVertices[6].position;	//	奥左下
	vertices[15].position = tempVertices[2].position;	//	手前左下
	vertices[12].normal = vertices[13].normal = vertices[14].normal = vertices[15].normal = { -1, 0, 0 };

	//	奥
	vertices[16].position = tempVertices[5].position;	//	奥右上
	vertices[17].position = tempVertices[4].position;	//	奥左上
	vertices[18].position = tempVertices[7].position;	//	奥右下
	vertices[19].position = tempVertices[6].position;	//	奥左下
	vertices[16].normal = vertices[17].normal = vertices[18].normal = vertices[19].normal = { 0, 0, 1 };

	//	下
	vertices[20].position = tempVertices[6].position;	//	奥左下
	vertices[21].position = tempVertices[7].position;	//	奥右下
	vertices[22].position = tempVertices[2].position;	//	手前左下
	vertices[23].position = tempVertices[3].position;	//	手前右下
	vertices[20].normal = vertices[21].normal = vertices[22].normal = vertices[23].normal = { 0, -1, 0 };


	//	面6 × 三角形2 = 12(面の三角形)
	//	その頂点数 12 * 3 = 36
	uint32_t indices[36]{};

	//	手前
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	indices[3] = 1;
	indices[4] = 3;
	indices[5] = 2;

	//	上
	indices[6] = 4;
	indices[7] = 5;
	indices[8] = 6;

	indices[9]  = 5;
	indices[10] = 7;
	indices[11] = 6;

	//	右
	indices[12] = 8;
	indices[13] = 9;
	indices[14] = 10;

	indices[15] = 9;
	indices[16] = 11;
	indices[17] = 10;

	//	左
	indices[18] = 12;
	indices[19] = 13;
	indices[20] = 14;

	indices[21] = 13;
	indices[22] = 15;
	indices[23] = 14;

	//	奥
	indices[24] = 16;
	indices[25] = 17;
	indices[26] = 18;

	indices[27] = 17;
	indices[28] = 19;
	indices[29] = 18;

	//	下
	indices[30] = 21;
	indices[31] = 20;
	indices[32] = 22;

	indices[33] = 21;
	indices[34] = 22;
	indices[35] = 23;


	createCOMBuffers(device, vertices, 24, indices, 36);

	HRESULT hr{ S_OK };

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]{

		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	Graphics::createVSAndILFromCSO(device, "./Data/Shaders/geometricPrimitiveVS.cso",
		vertexShader.GetAddressOf(), inputLayout.GetAddressOf(),
		inputElementDesc, ARRAYSIZE(inputElementDesc));
	Graphics::createPSFromCSO(device, "./Data/Shaders/geometricPrimitivePS.cso",
		pixelShader.GetAddressOf());

	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(Constants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


void GeometricPrimitive::createCOMBuffers(
	ID3D11Device* device, Vertex* vertices, size_t vertexCount,
	uint32_t* indices, size_t indexCount) {

	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC bufferDesc{};
	D3D11_SUBRESOURCE_DATA subResourceData{};
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertexCount);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	subResourceData.pSysMem = vertices;
	subResourceData.SysMemPitch = 0;
	subResourceData.SysMemSlicePitch = 0;
	
	hr = device->CreateBuffer(&bufferDesc, &subResourceData,
		vertexBuffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * indexCount);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subResourceData.pSysMem = indices;

	hr = device->CreateBuffer(&bufferDesc, &subResourceData,
		indexBuffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


void GeometricPrimitive::render(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext,
	const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor) {

	uint32_t stride{ sizeof(Vertex) };
	uint32_t offset{ 0 };

	immediateContext->IASetVertexBuffers(0, 1,
		vertexBuffer.GetAddressOf(), &stride, &offset);
	immediateContext->IASetIndexBuffer(
		indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->IASetInputLayout(inputLayout.Get());

	immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	Constants data{ world, materialColor };
	immediateContext->UpdateSubresource(
		constantBuffer.Get(), 0, 0, &data, 0, 0);
	immediateContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	D3D11_BUFFER_DESC bufferDesc{};
	indexBuffer->GetDesc(&bufferDesc);
	immediateContext->DrawIndexed(bufferDesc.ByteWidth / sizeof(uint32_t), 0, 0);
}