#include "DebugLineRenderer.h"

#include <vector>

#include "Graphics.h"
#include "../Library/Matrix.h"

// ===== �f�o�b�O���C���`��N���X ========================================================================================================================
DebugLineRenderer::DebugLineRenderer(ID3D11Device* device, int lineNum)
{
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	Graphics::createVSAndILFromCSO(device, "./Data/Shaders/DebugRendererVS.cso", vertexShader_.GetAddressOf(), inputLayout_.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	Graphics::createPSFromCSO(device, "./Data/Shaders/DebugRendererPS.cso", pixelShader_.GetAddressOf());

	std::vector<Vector3> vertices;

	for (int i = -(lineNum / 2); i <= (lineNum / 2); i++)
	{
		vertices.emplace_back(Vector3(i, 0,   lineNum / 2));
		vertices.emplace_back(Vector3(i, 0, -(lineNum / 2)));
		vertices.emplace_back(Vector3(  lineNum / 2,  0, i));
		vertices.emplace_back(Vector3(-(lineNum / 2), 0, i));
	}

	vertexCount_ = vertices.size();

	Graphics::createVertexBuffer(device, vertexBuffer_.GetAddressOf(), static_cast<UINT>(vertexCount_ * sizeof(Vector3)), D3D11_USAGE_DEFAULT, vertices.data());
}


void DebugLineRenderer::draw(ID3D11DeviceContext* dc)
{
	// --- �V�F�[�_�[�ݒ� ---
	dc->VSSetShader(vertexShader_.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader_.Get(), nullptr, 0);
	dc->IASetInputLayout(inputLayout_.Get());

	// --- ���_�o�b�t�@�ݒ� ---
	UINT stride{ sizeof(Vector3) };
	UINT offset{ 0 };
	dc->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);

	// --- �萔�o�b�t�@�ݒ� ---
	dc->VSSetConstantBuffers(0, 1, Graphics::instance().constantBuffers[Constants::OBJECT].GetAddressOf());
	dc->VSSetConstantBuffers(1, 1, Graphics::instance().constantBuffers[Constants::SCENE].GetAddressOf());

	// --- �`��@�ݒ� ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// --- ���[���h�s��̍쐬 ---
	Matrix S;
	S.makeScaling(Vector3::Unit_);
	Matrix R;
	R.makeRotation(Vector3::Zero_);
	Matrix T;
	T.makeTranslation(Vector3::Zero_);
	Matrix W = S * R * T;

	// --- �I�u�W�F�N�g�萔�o�b�t�@�X�V ---
	Constants::Object cb;
	cb.world_ = W.mat_;
	cb.color_ = Vector4::Black_;
	dc->UpdateSubresource(Graphics::instance().constantBuffers[Constants::OBJECT].Get(), 0, 0, &cb, 0, 0);

	dc->Draw(vertexCount_, 0);
}
