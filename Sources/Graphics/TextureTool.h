#pragma once

#include <wrl.h>
#include <string>
#include <map>

#include <d3d11.h>

HRESULT makeDummyTexture(Microsoft::WRL::ComPtr<ID3D11Device> device,
	ID3D11ShaderResourceView** shaderResourceView,
	DWORD value/*0xAABBGGRR*/, UINT dimension);

HRESULT loadTextureFromFile(Microsoft::WRL::ComPtr<ID3D11Device> device, const wchar_t* fileName,
	ID3D11ShaderResourceView** shaderResourceView, D3D11_TEXTURE2D_DESC* texture2dDesc);
