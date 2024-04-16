#pragma once

#include <memory>

#include <d3d11.h>
#include <wrl/client.h>
#include <dxgi1_6.h>
#include <d2d1_1.h>
#include <dwrite.h>
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")

#include "../Library/Matrix.h"
#include "../misc.h"
#include "DebugRenderer.h"
#include "DebugLineRenderer.h"
#include "SpriteRenderer.h"
#include "primitive.h"
#include "Illumination.h"

CONST LPCWSTR APPLICATION_NAME{ L" " };


// ===== �O���t�B�b�N�X�N���X ==================================================================================================================================
class Graphics {
private:
	Graphics();

public:
	// --- �C���X�^���X�̎擾 ---
	static Graphics& instance() {
		static Graphics instance_;
		return instance_;
	}

	void initialize();


	// --- DirectX11�֘A ---
	ID3D11Device* getDevice() { return device.Get(); }
	ID3D11DeviceContext* getDeviceContext() { return immediateContext.Get(); }
	IDXGISwapChain1* getSwapChain() { return swapChain_.Get(); }
	ID3D11RenderTargetView* getRenderTargetView() { return renderTargetView.Get(); }
	ID3D11DepthStencilView* getDepthStencilView() { return depthStencilView.Get(); }
	IDXGIAdapter3* getAdapter() { return adapter_.Get(); }

	// --- Direct2D�֘A ---
	ID2D1DeviceContext* getD2D1DeviceContext() { return d2d1DeviceContext_.Get(); }
	IDWriteTextFormat* getDWriteTextFormat(size_t index) { return dwriteTextFormats_[index].Get(); }
	ID2D1SolidColorBrush* getD2DSolidColorBrush(size_t index) { return d2dSolidColorBrushes_[index].Get(); }


	// ===== �V�F�[�_�[�֘A ==================================================================================================================================

	// --- ���_�V�F�[�_�[�̍쐬 ---
	static void createVSFromCSO(ID3D11Device* device, const char* CSOName, ID3D11VertexShader** vertexShader);


	// --- ���_�V�F�[�_�[�Ɠ��̓��C�A�E�g�̍쐬 ---
	static void createVSAndILFromCSO(ID3D11Device* device, const char* CSOName,
		ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout,
		D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements);


	// --- �s�N�Z���V�F�[�_�[�̍쐬 ---
	static void createPSFromCSO(ID3D11Device* device, const char* CSOName, ID3D11PixelShader** pixelShader);


	// --- �W�I���g���V�F�[�_�[�̍쐬 ---
	static void createGSFromCSO(ID3D11Device* device, const char* CSOName, ID3D11GeometryShader** geometryShader);


	// --- �R���s���[�g�V�F�[�_�[�̍쐬 ---
	static void createCSFromCSO(ID3D11Device* device, const char* CSOName, ID3D11ComputeShader** computeShader);


	// ===== �o�b�t�@�֘A ==================================================================================================================================
	
	// --- ���_�o�b�t�@�̍쐬 ---
	template <typename T>
	static void createVertexBuffer(ID3D11Device* device, ID3D11Buffer** vb, UINT byteSize, D3D11_USAGE usage, T* pData)
	{
		D3D11_BUFFER_DESC bufferDesc{};
		D3D11_SUBRESOURCE_DATA subResourceData{};
		{
			bufferDesc.ByteWidth = byteSize;
			bufferDesc.Usage = usage;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
		
			subResourceData.pSysMem = pData;
			subResourceData.SysMemPitch = 0;
			subResourceData.SysMemSlicePitch = 0;
		
			HRESULT hr =
				device->CreateBuffer(
					&bufferDesc, &subResourceData, vb
				);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

	}


	// --- �C���f�b�N�X�o�b�t�@�̍쐬 ---
	static void createIndexBuffer(ID3D11Device* device, ID3D11Buffer** ib, UINT byteSize, uint32_t* pData);


	// --- �萔�o�b�t�@�̍쐬 ---
	static void createConstantBuffer(ID3D11Device* device, ID3D11Buffer** cb, UINT byteSize);

	// --- �萔�o�b�t�@�̍X�V ---
	template <typename T>
	static void updateConstantBuffer(ID3D11DeviceContext* dc, ID3D11Buffer* cb, T& data)
	{
		dc->UpdateSubresource(cb, 0, 0, &data, 0, 0);
	}

	template<typename T>
	void updateConstantBuffer(int index, T& data)
	{
		immediateContext->UpdateSubresource(constantBuffers[index].Get(), 0, 0, &data, 0, 0);
	}


	// --- �e�N�X�`���̓ǂݍ��� ---
	static void loadTextureFromFile(ID3D11Device* device, const wchar_t* fileName, ID3D11ShaderResourceView** srv, D3D11_TEXTURE2D_DESC* tex2DDesc);


	// --- �`��N���X�֘A ---
	DebugRenderer* getDebugRenderer() const { return debugRenderer_.get(); }
	DebugLineRenderer* getDebugLineRenderer() const { return debugLineRenderer_.get(); }
	SpriteRenderer* getSpriteRenderer() const { return spriteRenderer_.get(); }
	Primitive* getPrimitiveRenderer() const { return primitiveRenderer_.get(); }

	
	// --- �Ǘ��N���X�֘A ---
	IlluminationManager* getIlluminationManager() const { return illumination_.get(); }


	void resetD2D1DeviceContext() { d2d1DeviceContext_.Reset(); }


	void setRasterizeState(int state);
	void setBlendState(int state);
	void setDepthStencil(int state);
	void setDepthStencil(bool test, bool write);
	void setSamplerState();
	void setRenderTarget();


	void createAdapter(IDXGIFactory6* dxgiFactory6);
	void createDevice();
	void createSwapChain(IDXGIFactory6* dxgiFactory6);
	void createRenderTargetView();
	void createDepthStencilView();
	void createViewport();
	void createSamplerState();
	void createDepthStencilState();
	void createBlendState();
	void createRasterizerState();
	void createDirect2DObjects();
	void clearRenderTargetView(const DirectX::XMFLOAT4& color);
	void clearDepthStencilView();


	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStates[4];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates[2/*�[�x�e�X�g*/][2/*�[�x��������*/];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> maskDepthStencilStates[4];
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendStates[4];
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffers[8];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[4];


private:
	// --- CSO�t�@�C���̓ǂݍ��� ---
	static void loadCSOFile(const char* CSOName, std::unique_ptr<unsigned char[]>& CSOData, long& CSOSize);

	// --- ���_�V�F�[�_�[�̍쐬 ---
	static void createVS(ID3D11Device* device, const unsigned char* CSOData, size_t CSOSize, ID3D11VertexShader** vertexShader);

	// --- �s�N�Z���V�F�[�_�[�̍쐬 ---
	static void createPS(ID3D11Device* device, const unsigned char* CSOData, size_t CSOSize, ID3D11PixelShader** pixelShader);

	// --- �W�I���g���V�F�[�_�[�̍쐬 ---
	static void createGS(ID3D11Device* device, const unsigned char* CSOData, size_t CSOSize, ID3D11GeometryShader** geometryShader);

	// --- �R���s���[�g�V�F�[�_�[�̍쐬 ---
	static void createCS(ID3D11Device* device, const unsigned char* CSOData, size_t CSOSize, ID3D11ComputeShader** computeShader);

	// --- ���̓��C�A�E�g�̍쐬 ---
	static void createIL(ID3D11Device* device, const D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements, const unsigned char* CSOData, size_t CSOSize, ID3D11InputLayout** inputLayout);


	// --- DirectX11�֘A ---
	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain1>			swapChain_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;
	Microsoft::WRL::ComPtr<IDXGIAdapter3> adapter_;

	// --- Direct2D�֘A ---
	Microsoft::WRL::ComPtr<ID2D1DeviceContext>		d2d1DeviceContext_;
	Microsoft::WRL::ComPtr<IDWriteTextFormat>		dwriteTextFormats_[8];
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>	d2dSolidColorBrushes_[8];

	// --- �����_���[�֘A ---
	std::unique_ptr<DebugRenderer>		debugRenderer_;
	std::unique_ptr<DebugLineRenderer>	debugLineRenderer_;
	std::unique_ptr<SpriteRenderer>		spriteRenderer_;
	std::unique_ptr<Primitive>			primitiveRenderer_;

	// --- �Ǘ��N���X�֘A ---
	std::unique_ptr<IlluminationManager> illumination_;
};


enum class BlendState { NONE, ALPHA, ADD };

enum class RasterState { CULL_NONE, SOLID, WIREFRAME, WIREFRAME_CULL_NONE };


// --- �萔�o�b�t�@�\���� ---
struct Constants {

	// --- �萔�̃��x�� ---
	enum { OBJECT, SCENE, MATERIAL, DISSOLVE, WINDOW, SHADER_TOY, };

	// --- �I�u�W�F�N�g�萔 ---
	struct Object
	{
		Matrix world_;		// ���[���h���W
		Vector4	color_;					// �F

	};

	// --- �V�[���萔 ---
	struct Scene
	{
		Matrix	viewProjection_;	//	�r���[�E�v���W�F�N�V�����ϊ��s��
		Matrix invViewProjection_;	//	�r���[�v���W�F�N�V�����̋t�s��
		Vector3	cameraPosition_;	//	�J�����̈ʒu
	};


	// --- �}�e���A���萔 ---
	struct Material
	{
		float specular_{ 128 };	//	�X�y�L�����[
	};


	// --- �f�B�]���u�萔 ---
	struct Dissolve
	{
		float amount_;
	};

	// --- �E�B���h�E�萔 ---
	struct Window
	{
		float width_;
		float height_;
	};

	// --- �V�F�[�_�[�g�C�萔 ---
	struct ShaderToy
	{
		Vector4 iMouse_;
		Vector4 iChannelResolution_[4];
		Vector4 iResolution_;
		float iTime_;
		float iFrame_;
	};
};