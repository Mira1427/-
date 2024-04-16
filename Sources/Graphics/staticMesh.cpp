#include "staticMesh.h"

#include <vector>
#include <fstream>
#include <filesystem>

#include <WICTextureLoader.h>

#include "../misc.h"
#include "TextureTool.h"
#include "Graphics.h"

StaticMesh::StaticMesh(ID3D11Device* device, const wchar_t* OBJ_fileName, const bool& isFlippedVertical) {

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	uint32_t currentIndex{ 0 };

	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> texcoords;
	std::vector<std::wstring>	   MTL_fileNames;

	std::wifstream fin(OBJ_fileName);
	_ASSERT_EXPR(fin, L" OBJ name not found");

	wchar_t command[256];

	while (fin) {

		fin >> command;

		if (0 == wcscmp(command, L"v")) {

			float x, y, z;
			fin >> x >> y >> z;
			positions.push_back({ x, y, z });
			fin.ignore(1024, L'\n');
		}

		else if (0 == wcscmp(command, L"vn")) {

			float i, j, k;
			fin >> i >> j >> k;
			normals.push_back({ i, j, k });
			fin.ignore(1024, L'\n');
		}

		else if (0 == wcscmp(command, L"vt")) {

			float u, v;
			fin >> u >> v;
			(isFlippedVertical) ? texcoords.push_back({ u, 1.0f - v })
				: texcoords.push_back({ u, v });
			fin.ignore(1024, L'\n');
		}

		else if (0 == wcscmp(command, L"f")) {

			for (size_t i = 0; i < 3; i++) {

				Vertex vertex;
				size_t v, vt, vn;

				fin >> v;
				vertex.position = positions.at(v - 1);

				if (L'/' == fin.peek()) {

					fin.ignore(1);

					if (L'/' != fin.peek()) {

						fin >> vt;
						vertex.texcoord = texcoords.at(vt - 1);
					}

					if (L'/' == fin.peek()) {

						fin.ignore(1);
						fin >> vn;
						vertex.normal = normals.at(vn - 1);
					}
				}

				vertices.push_back(vertex);
				indices.push_back(currentIndex++);
			}

			fin.ignore(1024, L'\n');
		}

		else if (0 == wcscmp(command, L"mtllib")) {

			wchar_t MTL_LIB[256];
			fin >> MTL_LIB;
			MTL_fileNames.push_back(MTL_LIB);
		}

		else if (0 == wcscmp(command, L"usemtl")) {

			wchar_t useMTL[MAX_PATH]{ 0 };
			fin >> useMTL;
			subsets.push_back({ useMTL, static_cast<uint32_t>(indices.size()), 0 });
		}

		else {

			fin.ignore(1024, L'\n');
		}
	}

	fin.close();


	//	MTLファイルパーサー
	std::filesystem::path MTL_fileName(OBJ_fileName);
	MTL_fileName.replace_filename(std::filesystem::path(MTL_fileNames[0]).filename());

	fin.open(MTL_fileName);
	//_ASSERT_EXPR(fin, L"MTL file not found");

	if (materials.size() == 0) {

		for (const Subset& subset : subsets) {

			materials.push_back({ subset.useMTL });
		}
	}

	while (fin) {

		fin >> command;

		if (0 == wcscmp(command, L"map_Kd")) {

			fin.ignore();
			wchar_t mapKd[256];
			fin >> mapKd;

			std::filesystem::path path(OBJ_fileName);
			path.replace_filename(std::filesystem::path(mapKd).filename());
			materials.rbegin()->textureFileNames[0] = path;
			fin.ignore(1024, L'\n');
		}

		else if (0 == wcscmp(command, L"map_bump") || 0 == wcscmp(command, L"bump")) {

			fin.ignore();
			wchar_t mapBump[256];
			fin >> mapBump;
			std::filesystem::path path(OBJ_fileName);
			path.replace_filename(std::filesystem::path(mapBump).filename());
			materials.rbegin()->textureFileNames[1] = path;
			fin.ignore(1024, L'\n');
		}

		else if (0 == wcscmp(command, L"newmtl")) {

			fin.ignore();
			wchar_t newMTL[256];
			Material material;
			fin >> newMTL;
			material.name = newMTL;
			materials.push_back(material);
		}

		else if (0 == wcscmp(command, L"Kd")) {

			float r, g, b;
			fin >> r >> g >> b;
			materials.rbegin()->Kd = { r, g, b, 1 };
			fin.ignore(1024, L'\n');
		}

		else if (0 == wcscmp(command, L"Ka")) {

			float r, g, b;
			fin >> r >> g >> b;
			materials.rbegin()->Ka = { r, g, b, 1 };
			fin.ignore(1024, L'\n');
		}
		
		else if (0 == wcscmp(command, L"Ks")) {

			float r, g, b;
			fin >> r >> g >> b;
			materials.rbegin()->Ks = { r, g, b, 1 };
			fin.ignore(1024, L'\n');
		}


		else {

			fin.ignore(1024, L'\n');
		}
	}

	fin.close();


	std::vector<Subset>::reverse_iterator iterator = subsets.rbegin();
	iterator->indexCount = static_cast<uint32_t>(indices.size()) - iterator->indexStart;

	for (iterator = subsets.rbegin() + 1; iterator != subsets.rend(); iterator++) {

		iterator->indexCount = (iterator - 1)->indexStart - iterator->indexStart;
	}


	//	すべてのマテリアルのテクスチャをロードし、シェーダーリソースビューを生成
	D3D11_TEXTURE2D_DESC texture2dDesc{};
	for (Material& material : materials) {

		Graphics::loadTextureFromFile(
			Graphics::instance().getDevice(),
			material.textureFileNames[0].c_str(),
			material.shaderResourceViews[0].GetAddressOf(),
			&texture2dDesc
		);
		//loadTextureFromFile(device, material.textureFileNames[0].c_str(),
		//	material.shaderResourceViews[0].GetAddressOf(), &texture2dDesc);

		//loadTextureFromFile(device.Get(), material.textureFileNames[1].c_str(),
		//	material.shaderResourceViews[1].GetAddressOf(), &texture2dDesc);
	}


	createComBuffer(device, vertices.data(), vertices.size(),
		indices.data(), indices.size());


	//	シェーダーオブジェクトの生成
	HRESULT hr{ S_OK };

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]{

		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	Graphics::createVSAndILFromCSO(device, "./Data/Shaders/staticMeshVS.cso",
		vertexShader.GetAddressOf(), inputLayout.GetAddressOf(),
		inputElementDesc, ARRAYSIZE(inputElementDesc));
	Graphics::createPSFromCSO(device, "./Data/Shaders/staticMeshPS.cso",
		pixelShader.GetAddressOf());

	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(Constants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


void StaticMesh::createComBuffer(
	ID3D11Device* device,
	Vertex* vertices, size_t vertexCount,
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


void StaticMesh::render(
	ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT4X4& world,
	const DirectX::XMFLOAT4& materialColor) {

	uint32_t stride{ sizeof(Vertex) };
	uint32_t offset{ 0 };

	immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	immediateContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->IASetInputLayout(inputLayout.Get());

	immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	immediateContext->VSSetConstantBuffers(1, 1, Graphics::instance().constantBuffers[1].GetAddressOf());
	immediateContext->PSSetConstantBuffers(2, 1, Graphics::instance().getIlluminationManager()->constantBuffer_.GetAddressOf());

	for (const Material& material : materials) {

		immediateContext->PSSetShaderResources(0, 1,
			material.shaderResourceViews[0].GetAddressOf());

		immediateContext->PSSetShaderResources(1, 1,
			material.shaderResourceViews[1].GetAddressOf());

		Constants data{ world, materialColor };
		DirectX::XMStoreFloat4(&data.materialColor,
			DirectX::XMVectorMultiply(DirectX::XMLoadFloat4(&materialColor), DirectX::XMLoadFloat4(&material.Kd)));
		immediateContext->UpdateSubresource(
			constantBuffer.Get(), 0, 0, &data, 0, 0);
		immediateContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

		for (const Subset& subset : subsets) {

			if (material.name == subset.useMTL) {

				immediateContext->DrawIndexed(subset.indexCount, subset.indexStart, 0);
			}
		}
	}
}


//HRESULT makeDummy(Microsoft::WRL::ComPtr<ID3D11Device> device,
//	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView*>shaderResourceView,
//	DWORD value/*0xAABBGGRR*/, UINT dimension) {
//
//	HRESULT hr{ S_OK };
//
//	D3D11_TEXTURE2D_DESC texture2dDesc{};
//	texture2dDesc.Width = dimension;
//	texture2dDesc.Height = dimension;
//	texture2dDesc.MipLevels = 1;
//	texture2dDesc.ArraySize = 1;
//	texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	texture2dDesc.SampleDesc.Count = 1;
//	texture2dDesc.SampleDesc.Quality = 0;
//	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
//	texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//
//
//	size_t texels = dimension * dimension;
//	std::unique_ptr<DWORD[]> sysmem{ std::make_unique<DWORD[]>(texels) };
//	for (size_t i = 0; i < texels; ++i) sysmem[i] = value;
//
//	D3D11_SUBRESOURCE_DATA subresourceData{};
//	subresourceData.pSysMem = sysmem.get();
//	subresourceData.SysMemPitch = sizeof(DWORD) * dimension;
//
//	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
//	hr = device->CreateTexture2D(&texture2dDesc, &subresourceData, texture2d.GetAddressOf());
//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//	return hr;
//}