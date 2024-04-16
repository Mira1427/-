#include <wrl.h>
#include <cstdio>
#include <memory>

#include <WICTextureLoader.h>

#include "../misc.h"

HRESULT createVSFromCSO(ID3D11Device* device, const char* CSOName,
	ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout,
	D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements) {

	FILE* fp{ nullptr };
	fopen_s(&fp, CSOName, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long CSOSize{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> 
		CSOData{ std::make_unique<unsigned char[]>(CSOSize) };
	fread(CSOData.get(), CSOSize, 1, fp);
	fclose(fp);

	HRESULT hr{ S_OK };
	hr = device->CreateVertexShader(
		CSOData.get(), CSOSize, nullptr, vertexShader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (inputLayout) {

		hr = device->CreateInputLayout(inputElementDesc, numElements,
			CSOData.get(), CSOSize, inputLayout);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	return hr;
}


HRESULT createPSFromCSO(ID3D11Device* device,
	const char* CSOName, ID3D11PixelShader** pixelShader) {

	FILE* fp{ nullptr };
	fopen_s(&fp, CSOName, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long CSOSize{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]>
		CSOData{ std::make_unique<unsigned char[]>(CSOSize) };
	fread(CSOData.get(), CSOSize, 1, fp);
	fclose(fp);

	HRESULT hr{ S_OK };
	hr = device->CreatePixelShader(CSOData.get(),
		CSOSize, nullptr, pixelShader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return hr;
}