#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "shader.h"
#include <string>
#include <vector>


class StaticMesh {

public:
	struct Vertex {

		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
	};

	struct Constants {

		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 materialColor;
	};

	struct Subset {

		std::wstring useMTL;
		uint32_t indexStart{ 0 };	//	start position of index buffer
		uint32_t indexCount{ 0 };	//	number of vertices (indices)
	};

	std::vector<Subset> subsets;


private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;


	struct Material {

		std::wstring name;
		DirectX::XMFLOAT4 Ka{ 0.2f, 0.2f, 0.2f, 1.0f };		//	ŠÂ‹«Œõ‚É‘Î‚·‚é”½ŽË—¦
		DirectX::XMFLOAT4 Kd{ 0.8f, 0.8f, 0.8f, 1.0f };		//	ŠgŽU”½ŽË‚É‘Î‚·‚é”½ŽË—¦
		DirectX::XMFLOAT4 Ks{ 1.0f, 1.0f, 1.0f, 1.0f };		//	‹¾–Ê”½ŽË‚É‘Î‚·‚é”½ŽË—¦
		std::wstring textureFileNames[2];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[2];
	};

public:
	std::vector<Material> materials;

public:
	StaticMesh(ID3D11Device* device, const wchar_t* objFileName, const bool& isFlippedVertical);
	virtual ~StaticMesh() = default;

	void render(ID3D11DeviceContext* immediateContext,
		const DirectX::XMFLOAT4X4& world,
		const DirectX::XMFLOAT4& materialColor);


protected:
	void createComBuffer(ID3D11Device* device,
		Vertex* vertices, size_t vertexCount,
		uint32_t* indices, size_t indexCount);
};