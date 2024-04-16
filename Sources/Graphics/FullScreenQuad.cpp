#include "FullScreenQuad.h"
#include "../misc.h"
#include "Graphics.h"


FullScreenQuad::FullScreenQuad(ID3D11Device* device)
{
	Graphics::createVSFromCSO(device, "./Data/Shaders/FullScreenQuadVS.cso", embeddedVertexShader.ReleaseAndGetAddressOf());
	Graphics::createPSFromCSO(device, "./Data/Shaders/FullScreenQuadPS.cso", embeddedPixelShader.ReleaseAndGetAddressOf());
}


void FullScreenQuad::blit(
	ID3D11DeviceContext* immediateContext,
	ID3D11ShaderResourceView** shaderResourceView,
	uint32_t startSlot,
	uint32_t numViews,
	ID3D11PixelShader* replacedPixelShader)
{

	immediateContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediateContext->IASetInputLayout(nullptr);


	immediateContext->VSSetShader(embeddedVertexShader.Get(), 0, 0);
	replacedPixelShader ?
		immediateContext->PSSetShader(replacedPixelShader, 0, 0) :
		immediateContext->PSSetShader(embeddedPixelShader.Get(), 0, 0);

	immediateContext->PSSetShaderResources(startSlot, numViews, shaderResourceView);

	immediateContext->Draw(4, 0);
}