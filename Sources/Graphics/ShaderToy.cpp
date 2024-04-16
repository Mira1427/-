#include "ShaderToy.h"
#include "Graphics.h"


ShaderToy::ShaderToy(ID3D11Device* device) {

	Graphics::createVSFromCSO(device, "./Data/Shaders/ShaderToyVS.cso", embeddedVertexShader_.ReleaseAndGetAddressOf());
	Graphics::createPSFromCSO(device, "./Data/Shaders/ShaderToyPS.cso", embeddedPixelShader_.ReleaseAndGetAddressOf());
}


void ShaderToy::blit(ID3D11DeviceContext* immediateContext, float time, 
	ID3D11ShaderResourceView** shaderResourceView, 
	uint32_t startSlot, uint32_t numViews, 
	ID3D11PixelShader* replacedPixelShader) {

	D3D11_VIEWPORT viewport;
	UINT numViewports{ 1 };
	immediateContext->RSGetViewports(&numViewports, &viewport);

	immediateContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediateContext->IASetInputLayout(nullptr);

	immediateContext->VSSetShader(embeddedVertexShader_.Get(), 0, 0);
	replacedPixelShader ?
		immediateContext->PSSetShader(replacedPixelShader, 0, 0) :
		immediateContext->PSSetShader(embeddedPixelShader_.Get(), 0, 0);

	immediateContext->PSSetShaderResources(startSlot, numViews, shaderResourceView);

	immediateContext->Draw(4, 0);
}
