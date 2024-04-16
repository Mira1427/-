#include "primitive.h"
#include "../misc.h"

#include <sstream>
#include <WICTextureLoader.h>


Primitive::Primitive(ID3D11Device* device) {

	//	���_���̃Z�b�g
	Vertex vertices[]{

		{ { -1.0, +1.0, 0 }, {1, 1, 1, 1} },	
		{ { +1.0, +1.0, 0 }, {1, 1, 1, 1} },	
		{ { -1.0, -1.0, 0 }, {0.5, 0.5, 0.5, 1} },	
		{ { +1.0, -1.0, 0 }, {0.5, 0.5, 0.5, 1} }
	};

	HRESULT hr{ S_OK };

	//	���_�o�b�t�@�I�u�W�F�N�g�̐���
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pSysMem = vertices;
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//	���_�V�F�[�_�[�I�u�W�F�N�g�̐���
	const char* VS_CSO_Name("./Data/Shaders/primitiveVS.cso");

	FILE* VS_FP{};
	fopen_s(&VS_FP, VS_CSO_Name, "rb");
	_ASSERT_EXPR_A(VS_FP, "CSO File not found");

	fseek(VS_FP, 0, SEEK_END);
	long VS_CSO_Size{ ftell(VS_FP) };
	fseek(VS_FP, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> VS_CSO_Data{ std::make_unique<unsigned char[]>(VS_CSO_Size) };
	fread(VS_CSO_Data.get(), VS_CSO_Size, 1, VS_FP);
	fclose(VS_FP);

	hr = device->CreateVertexShader(VS_CSO_Data.get(), VS_CSO_Size, nullptr, vertexShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//	���̓��C�A�E�g�I�u�W�F�N�g�̐���
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]{

		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	hr = device->CreateInputLayout(inputElementDesc, _countof(inputElementDesc),
		VS_CSO_Data.get(), VS_CSO_Size, inputLayout.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//	�s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
	const char* PS_CSO_Name("./Data/Shaders/primitivePS.cso");

	FILE* PS_FP{};
	fopen_s(&PS_FP, PS_CSO_Name, "rb");
	_ASSERT_EXPR_A(PS_FP, "CSO File not found");

	fseek(PS_FP, 0, SEEK_END);
	long PS_CSO_Size{ ftell(PS_FP) };
	fseek(PS_FP, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> PS_CSO_Data{ std::make_unique<unsigned char[]>(PS_CSO_Size) };
	fread(PS_CSO_Data.get(), PS_CSO_Size, 1, PS_FP);
	fclose(PS_FP);

	hr = device->CreatePixelShader(PS_CSO_Data.get(), PS_CSO_Size, nullptr, pixelShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


void Primitive::render(
	ID3D11DeviceContext* immediateContext,
	const DirectX::XMFLOAT2& position,
	const DirectX::XMFLOAT2& size, const DirectX::XMFLOAT2& center,
	const float& angle, const DirectX::XMFLOAT4& color) const {

	//	�X�N���[���i�r���[�|�[�g�j�̃T�C�Y���擾����
	D3D11_VIEWPORT viewport{};
	UINT numViewport{ 1 };
	immediateContext->RSGetViewports(&numViewport, &viewport);


	//	render�����o�֐��̈�������A��`�̊e���_�̈ʒu���v�Z����
	//	(x0, y0) *----* (x1, y1)
	//			 |   /|
	//			 |  / |
	//			 | /  |
	//			 |/   |
	//	(x2, y2) *----* (x3, y3)

	//	Left_Top
	float x0{ position.x - center.x };
	float y0{ position.y - center.y };

	//	Right_Top
	float x1{ position.x + size.x - center.x };
	float y1{ position.y - center.y };

	//	Left_Bottom
	float x2{ position.x - center.x };
	float y2{ position.y + size.y - center.y };

	//	RIght_Bottom
	float x3{ position.x + size.x - center.x };
	float y3{ position.y + size.y - center.y };


	//	��]�̒��S����`�̒��S�_�ɂ����ꍇ
	float cx = position.x - center.x + size.x * 0.5f;
	float cy = position.y - center.y + size.y * 0.5f;
	rotate_c(x0, y0, cx, cy, angle);
	rotate_c(x1, y1, cx, cy, angle);
	rotate_c(x2, y2, cx, cy, angle);
	rotate_c(x3, y3, cx, cy, angle);


	//	�X�N���[�����W�n����NDC�ւ̕ϊ����s��
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;

	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;

	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;

	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;


	//	�v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = immediateContext->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Vertex* vertices{ reinterpret_cast<Vertex*>(mappedSubresource.pData) };

	if (vertices != nullptr) {

		vertices[0].position = { x0, y0, 0 };
		vertices[1].position = { x1, y1, 0 };
		vertices[2].position = { x2, y2, 0 };
		vertices[3].position = { x3, y3, 0 };

		vertices[0].color = vertices[1].color = vertices[2].color = 
			vertices[3].color = color;
	}

	immediateContext->Unmap(vertexBuffer.Get(), 0);

	//	�V�F�[�_�[���\�[�X�̃o�C���h
	//immediateContext->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

	//	���_�o�b�t�@�[�̃o�C���h
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);


	//	�v���~�e�B�u�^�C�v����уf�[�^�̏����Ɋւ�����̃o�C���h
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	//	���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
	immediateContext->IASetInputLayout(inputLayout.Get());


	//	�V�F�[�_�[�̃o�C���h
	immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);


	//	�v���~�e�B�u�̕`��
	immediateContext->Draw(4, 0);
}


void Primitive::render(ID3D11DeviceContext* immediateContext,
	const float& dx, const float& dy,
	const float& sizeX, const float& sizeY, const float& cx, const float& cy, 
	const float& angle,
	const float& r, const float& g, const float& b, const float& a) const {

	return render(immediateContext, DirectX::XMFLOAT2(dx, dy), 
		DirectX::XMFLOAT2(sizeX, sizeY), DirectX::XMFLOAT2(cx, cy), angle, DirectX::XMFLOAT4(r, g, b, a));
}


Primitive::~Primitive() {
}