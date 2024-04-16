#include "TextureTool.h"
#include "../misc.h"

#include <filesystem>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> resources;


HRESULT makeDummyTexture(Microsoft::WRL::ComPtr<ID3D11Device> device,
	ID3D11ShaderResourceView** shaderResourceView,
	DWORD value/*0xAABBGGRR*/, UINT dimension) {

	HRESULT hr{ S_OK };

	D3D11_TEXTURE2D_DESC texture2dDesc{};
	texture2dDesc.Width = dimension;
	texture2dDesc.Height = dimension;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;


	size_t texels = dimension * dimension;
	std::unique_ptr<DWORD[]> sysmem{ std::make_unique<DWORD[]>(texels) };
	for (size_t i = 0; i < texels; ++i) sysmem[i] = value;

	D3D11_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pSysMem = sysmem.get();
	subresourceData.SysMemPitch = sizeof(DWORD) * dimension;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	hr = device->CreateTexture2D(&texture2dDesc, &subresourceData, texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = texture2dDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(texture2d.Get(), &shaderResourceViewDesc,
		shaderResourceView);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return hr;
}


HRESULT loadTextureFromFile(Microsoft::WRL::ComPtr<ID3D11Device> device, const wchar_t* fileName,
	ID3D11ShaderResourceView** shaderResourceView, D3D11_TEXTURE2D_DESC* texture2dDesc) {

	HRESULT hr{ S_OK };
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;

	auto it = resources.find(fileName);

	if (it != resources.end()) {

		*shaderResourceView = it->second.Get();
		(*shaderResourceView)->AddRef();
		(*shaderResourceView)->GetResource(resource.GetAddressOf());
	}

	else {

		std::filesystem::path ddsFileName(fileName);
		ddsFileName.replace_extension("dds");

		if (std::filesystem::exists(ddsFileName.c_str())) {

			hr = DirectX::CreateDDSTextureFromFile(device.Get(), ddsFileName.c_str(), resource.GetAddressOf(), shaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		else {

			hr = DirectX::CreateWICTextureFromFile(device.Get(), fileName,
				resource.GetAddressOf(), shaderResourceView);
			//	Ç±Ç±Ç…Ç‘ÇøçûÇﬁ
			if (FAILED(hr)) {

				makeDummyTexture(device.Get(), shaderResourceView,
					0xFFFFFFFF, 16);

				//makeDummyTexture(device.Get(), shaderResourceView,
				//	0xFFFF7F7F, 16);
			}
			//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			resources.insert(std::make_pair(fileName, *shaderResourceView));
		}
	}

	//Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	//hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	//_ASSERT(SUCCEEDED(hr), hr_trace(hr));
	//texture2d->GetDesc(texture2dDesc);

	return hr;
}
