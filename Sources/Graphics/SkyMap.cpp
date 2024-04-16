#include "SkyMap.h"

#include "Graphics.h"


// --- �R���X�g���N�^ ---
SkyMap::SkyMap(ID3D11Device* device, const wchar_t* fileName)
{
	Graphics& graphics = Graphics::instance();

	// --- �e�N�X�`���̓ǂݍ��� ---
	D3D11_TEXTURE2D_DESC tex2DDesc{};
	Graphics::loadTextureFromFile(device, fileName, shaderResourceView_.GetAddressOf(), &tex2DDesc);

	if (tex2DDesc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
		isCubeTexture_ = true;

	// --- ���_�V�F�[�_�[�̍쐬 ---
	Graphics::createVSFromCSO(graphics.getDevice(), "./Data/Shaders/SkyMap_VS.cso", vertexShader_.GetAddressOf());

	// --- �s�N�Z���V�F�[�_�̍쐬 ---
	Graphics::createPSFromCSO(graphics.getDevice(), "./Data/Shaders/SkyMap_PS.cso", skyMapPS_.GetAddressOf());
	Graphics::createPSFromCSO(graphics.getDevice(), "./Data/Shaders/SkyBox_PS.cso", skyBoxPS_.GetAddressOf());
}


// --- �`�揈�� ---
void SkyMap::draw(ID3D11DeviceContext* dc)
{
	dc->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->IASetInputLayout(NULL);
	
	dc->VSSetShader(vertexShader_.Get(), 0, 0);
	dc->PSSetShader(isCubeTexture_ ? skyBoxPS_.Get() : skyMapPS_.Get(), 0, 0);

	dc->PSSetShaderResources(0, 1, shaderResourceView_.GetAddressOf());

	dc->PSSetConstantBuffers(0, 1, Graphics::instance().constantBuffers[Constants::SCENE].GetAddressOf());

	dc->Draw(4, 0);

	dc->VSSetShader(NULL, 0, 0);
	dc->PSSetShader(NULL, 0, 0);
}
