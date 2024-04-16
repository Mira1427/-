#include "Graphics.h"

#include <filesystem>

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

#include "../Library/fileManager.h"
#include "../Library/library.h"
#include "TextureTool.h"


// --- �R���X�g���N�^ ---
Graphics::Graphics()
{
}

// --- ������ ---
void Graphics::initialize()
{
	HRESULT hr = S_OK;
	UINT createFactoryFlags = {};

#ifdef _DEBUG
	createFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory6;

	hr = CreateDXGIFactory2(
		createFactoryFlags,
		IID_PPV_ARGS(dxgiFactory6.GetAddressOf())
	);

	createAdapter(dxgiFactory6.Get());
	createDevice();
	createSwapChain(dxgiFactory6.Get());
	createRenderTargetView();
	createDepthStencilView();
	createViewport();
	createSamplerState();
	createDepthStencilState();
	createBlendState();
	createRasterizerState();
	createDirect2DObjects();

	debugRenderer_		= std::make_unique<DebugRenderer>(device.Get());
	debugLineRenderer_	= std::make_unique<DebugLineRenderer>(device.Get(), 100);
	spriteRenderer_		= std::make_unique<SpriteRenderer>(device.Get());
	primitiveRenderer_	= std::make_unique<Primitive>(device.Get());

	illumination_ = std::make_unique<IlluminationManager>(device.Get());
}

// ===== �V�F�[�_�[�֘A =======================================================================================================================================

// --- CSO�t�@�C���̓ǂݍ��� ---
void Graphics::loadCSOFile(const char* CSOName, std::unique_ptr<unsigned char[]>& CSOData, long& CSOSize)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, CSOName, "rb");
	_ASSERT_EXPR_A(fp, L"CSO�t�@�C����������܂���ł���");

	fseek(fp, 0, SEEK_END);
	CSOSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	CSOData = std::make_unique<unsigned char[]>(CSOSize);
	fread(CSOData.get(), CSOSize, 1, fp);
	fclose(fp);
}


// --- ���_�V�F�[�_�[�̍쐬 ---
void Graphics::createVS(ID3D11Device* device, const unsigned char* CSOData, size_t CSOSize, ID3D11VertexShader** vertexShader)
{
	HRESULT hr =
		device->CreateVertexShader(
			CSOData, CSOSize, nullptr, vertexShader);

	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


// --- �s�N�Z���V�F�[�_�[�̍쐬 ---
void Graphics::createPS(ID3D11Device* device, const unsigned char* CSOData, size_t CSOSize, ID3D11PixelShader** pixelShader)
{
	HRESULT hr =
		device->CreatePixelShader(CSOData, CSOSize, nullptr, pixelShader);

	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


// --- �W�I���g���V�F�[�_�[�̍쐬 ---
void Graphics::createGS(ID3D11Device* device, const unsigned char* CSOData, size_t CSOSize, ID3D11GeometryShader** geometryShader)
{
	HRESULT hr =
		device->CreateGeometryShader(CSOData, CSOSize, nullptr, geometryShader);

	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


// --- �R���s���[�g�V�F�[�_�[�̍쐬 ---
void Graphics::createCS(ID3D11Device* device, const unsigned char* CSOData, size_t CSOSize, ID3D11ComputeShader** computeShader)
{
	HRESULT hr =
		device->CreateComputeShader(CSOData, CSOSize, nullptr, computeShader);

	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


// --- ���̓��C�A�E�g�̍쐬 ---
void Graphics::createIL(
	ID3D11Device* device, const D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements,
	const unsigned char* CSOData, size_t CSOSize, ID3D11InputLayout** inputLayout)
{
	HRESULT hr =
		device->CreateInputLayout(
			inputElementDesc, numElements, CSOData, CSOSize, inputLayout);

	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


// --- ���_�V�F�[�_�[�̍쐬 ---
void Graphics::createVSFromCSO(ID3D11Device* device, const char* CSOName, ID3D11VertexShader** vertexShader)
{
	std::unique_ptr<unsigned char[]> CSOData;
	long CSOSize{};

	// --- CSO�t�@�C���̓ǂݍ��� ---
	loadCSOFile(CSOName, CSOData, CSOSize);

	// --- ���_�V�F�[�_�[�̍쐬 ---
	createVS(device, CSOData.get(), static_cast<size_t>(CSOSize), vertexShader);
}


// --- ���_�V�F�[�_�[�Ɠ��̓��C�A�E�g�̍쐬 ---
void Graphics::createVSAndILFromCSO(ID3D11Device* device, const char* CSOName, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements)
{
	std::unique_ptr<unsigned char[]> CSOData;
	long CSOSize{};

	// --- CSO�t�@�C���̓ǂݍ��� ---
	loadCSOFile(CSOName, CSOData, CSOSize);

	// --- ���_�V�F�[�_�[�̍쐬 ---
	createVS(device, CSOData.get(), static_cast<size_t>(CSOSize), vertexShader);

	// --- ���̓��C�A�E�g�̍쐬 ---
	createIL(device, inputElementDesc, numElements, CSOData.get(), static_cast<size_t>(CSOSize), inputLayout);
}


// --- �s�N�Z���V�F�[�_�[�̍쐬 ---
void Graphics::createPSFromCSO(ID3D11Device* device, const char* CSOName, ID3D11PixelShader** pixelShader)
{
	std::unique_ptr<unsigned char[]> CSOData;
	long CSOSize{};

	// --- CSO�t�@�C���̓ǂݍ��� ---
	loadCSOFile(CSOName, CSOData, CSOSize);

	// --- �s�N�Z���V�F�[�_�[�̍쐬 ---
	createPS(device, CSOData.get(), static_cast<size_t>(CSOSize), pixelShader);
}


// --- �W�I���g���V�F�[�_�[�̍쐬 ---
void Graphics::createGSFromCSO(ID3D11Device* device, const char* CSOName, ID3D11GeometryShader** geometryShader)
{
	std::unique_ptr<unsigned char[]> CSOData;
	long CSOSize{};

	// --- CSO�t�@�C���̓ǂݍ��� ---
	loadCSOFile(CSOName, CSOData, CSOSize);

	// --- �W�I���g���V�F�[�_�[�̍쐬 ---
	createGS(device, CSOData.get(), static_cast<size_t>(CSOSize), geometryShader);
}


// --- �R���s���[�g�V�F�[�_�[�̍쐬 ---
void Graphics::createCSFromCSO(ID3D11Device* device, const char* CSOName, ID3D11ComputeShader** computeShader)
{
	std::unique_ptr<unsigned char[]> CSOData;
	long CSOSize{};

	// --- CSO�t�@�C���̓ǂݍ��� ---
	loadCSOFile(CSOName, CSOData, CSOSize);

	// --- �R���s���[�g�V�F�[�_�[�̍쐬 ---
	createCS(device, CSOData.get(), static_cast<size_t>(CSOSize), computeShader);
}



// ===== �o�b�t�@�֘A =======================================================================================================================================

// --- �C���f�b�N�X�o�b�t�@�̍쐬 ---
void Graphics::createIndexBuffer(ID3D11Device* device, ID3D11Buffer** ib, UINT byteSize, uint32_t* pData)
{
	D3D11_BUFFER_DESC bufferDesc{};
	D3D11_SUBRESOURCE_DATA subResourceData{};

	bufferDesc.ByteWidth = byteSize;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	subResourceData.pSysMem = pData;

	HRESULT hr =
		device->CreateBuffer(
			&bufferDesc, &subResourceData, ib
		);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

// --- �萔�o�b�t�@�̐��� ---
void Graphics::createConstantBuffer(ID3D11Device* device, ID3D11Buffer** cb, UINT byteSize) {

	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = byteSize + ((byteSize % 16 == 0) ? 0 : 16 - (byteSize % 16));	//	�o�b�t�@�̃o�C�g��
	desc.Usage = D3D11_USAGE_DEFAULT;												//	�o�b�t�@�̎g�p�@
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;									//	�o�b�t�@�̃o�C���h�t���O
	desc.CPUAccessFlags = 0;														//	CPU�̃A�N�Z�X�t���O
	desc.MiscFlags = 0;																//	���̑��̃t���O
	desc.StructureByteStride = 0;													//	�\���̂̃o�C�g�X�g���C�h

	// --- �o�b�t�@�̍쐬 ---
	HRESULT hr =
		device->CreateBuffer(
			&desc, nullptr, cb
		);

	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}



// ===== �e�N�X�`���֘A =======================================================================================================================================
void Graphics::loadTextureFromFile(ID3D11Device* device, const wchar_t* fileName, ID3D11ShaderResourceView** srv, D3D11_TEXTURE2D_DESC* tex2DDesc)
{
	HRESULT hr{ S_OK };
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;

	// --- dds�t�@�C�������p�Ɋg���q��ύX ---
	std::filesystem::path ddsFileName(fileName);
	ddsFileName.replace_extension("dds");

	// --- dds�t�@�C�������������� ---
	if (std::filesystem::exists(ddsFileName.c_str()))
	{
		hr = DirectX::CreateDDSTextureFromFile(
			device,
			ddsFileName.c_str(),
			resource.GetAddressOf(),
			srv
		);

		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// --- �@������Ȃ�������png�œǂݍ��� ---
	else
	{
		hr = DirectX::CreateWICTextureFromFile(
			device,
			fileName,
			resource.GetAddressOf(),
			srv
		);

		if (FAILED(hr))
		{
			hr = makeDummyTexture(device, srv, 0xFFFFFFFF, 16);
		}

		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	
	// --- Texture2DDesc�̎擾 ---
	if (tex2DDesc && resource)
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
		hr = resource.Get()->QueryInterface<ID3D11Texture2D>(tex2D.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		tex2D->GetDesc(tex2DDesc);
	}
}


// ===== �E�B���h�E�֘A =======================================================================================================================================

// --- �f�o�C�X�̍쐬 ---
void Graphics::createDevice()
{
	HRESULT hr = S_OK;
	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	createDeviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_1;
	hr = D3D11CreateDevice(
		adapter_.Get(),
		D3D_DRIVER_TYPE_UNKNOWN,
		0,
		createDeviceFlags,
		&featureLevels,
		1,
		D3D11_SDK_VERSION,
		device.GetAddressOf(),
		NULL,
		immediateContext.GetAddressOf()
	);

	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


// --- �X���b�v�`�F�C���̐��� ---
void Graphics::createSwapChain(IDXGIFactory6* dxgiFactory6)
{
	HRESULT hr{ S_OK };

	// �X���b�v�`�F�C�����쐬����Ă�����
	if (swapChain_)
	{
		ID3D11RenderTargetView* nullRenderTargetView = {};
		immediateContext.Get()->OMSetRenderTargets(1, &nullRenderTargetView, NULL);
		renderTargetView.Reset();

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChain_.Get()->GetDesc(&swapChainDesc);
		hr = swapChain_.Get()->ResizeBuffers(
			swapChainDesc.BufferCount,
			window::getWidth(),
			window::getHeight(),
			swapChainDesc.BufferDesc.Format,
			swapChainDesc.Flags
		);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBuffer;
		hr = swapChain_.Get()->GetBuffer(
			0,
			IID_PPV_ARGS(renderTargetBuffer.GetAddressOf())
		);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_TEXTURE2D_DESC texture2DDesc;
		renderTargetBuffer.Get()->GetDesc(&texture2DDesc);

		hr = device.Get()->CreateRenderTargetView(
			renderTargetBuffer.Get(),
			NULL,
			renderTargetView.ReleaseAndGetAddressOf()
		);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// --- ���쐬 ---
	else
	{
		if (swapChain_)
			swapChain_->Release();

		BOOL allowTearing = FALSE;

		if (SUCCEEDED(hr))
		{
			hr = dxgiFactory6->CheckFeatureSupport(
				DXGI_FEATURE_PRESENT_ALLOW_TEARING,
				&allowTearing,
				sizeof(allowTearing)
			);
		}

		window::setTdearingSupport(SUCCEEDED(hr) && allowTearing);

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc1 = {};
		{
			swapChainDesc1.Width = static_cast<UINT>(window::getWidth());
			swapChainDesc1.Height = static_cast<UINT>(window::getHeight());
			swapChainDesc1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc1.Stereo = FALSE;
			swapChainDesc1.SampleDesc.Count = 1;
			swapChainDesc1.SampleDesc.Quality = 0;
			swapChainDesc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc1.BufferCount = 2;
			swapChainDesc1.Scaling = DXGI_SCALING_STRETCH;
			swapChainDesc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			swapChainDesc1.Flags = (window::isTearingSupported() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);

			hr = dxgiFactory6->CreateSwapChainForHwnd(
				device.Get(),
				window::getHwnd(),
				&swapChainDesc1,
				NULL,
				NULL,
				swapChain_.ReleaseAndGetAddressOf()
			);

			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			// --- �Ȃ񂩓{���Ă� ---
			// IDXGIFactory::MakeWindowAssociation: The factory did not contain a swapchain associated with the provided HWND. To retrieve the correct factory, call GetParent() on the swapchain.
			hr = dxgiFactory6->MakeWindowAssociation(
				window::getHwnd(),
				DXGI_MWA_NO_ALT_ENTER
			);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
	}
}


// --- �����_�[�^�[�Q�b�g�r���[�̐��� ---
void Graphics::createRenderTargetView()
{
	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer{};
	hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = device->CreateRenderTargetView(backBuffer.Get(), NULL, renderTargetView.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


// --- �[�x�X�e���V���r���[�̐��� ---
void Graphics::createDepthStencilView()
{
	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer{};
	D3D11_TEXTURE2D_DESC texture2dDesc{};
	texture2dDesc.Width = window::getWidth();
	texture2dDesc.Height = window::getHeight();
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2dDesc.CPUAccessFlags = 0;
	texture2dDesc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2dDesc, NULL, depthStencilBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


// --- �r���[�|�[�g�̐ݒ� ---
void Graphics::createViewport()
{
	HRESULT hr{ S_OK };

	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = window::getWidth();
	viewport.Height = window::getHeight();
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	immediateContext->RSSetViewports(1, &viewport);
}


// --- �T���v���[�X�e�[�g�I�u�W�F�N�g�̐��� ---
void Graphics::createSamplerState()
{
	HRESULT hr{ S_OK };

	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&samplerDesc, samplerStates[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	hr = device->CreateSamplerState(&samplerDesc, samplerStates[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	hr = device->CreateSamplerState(&samplerDesc, samplerStates[2].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	hr = device->CreateSamplerState(&samplerDesc, samplerStates[3].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


// --- �[�x�X�e���V���X�e�[�g�̐��� ---
void Graphics::createDepthStencilState()
{
	HRESULT hr{ S_OK };

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};


	//	�[�x�e�X�g�F�I��		�[�x���C�g�F�I��
	depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[1][1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//	�[�x�e�X�g�F�I�t		�[�x���C�g�F�I��
	depthStencilDesc = {};
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[0][1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//	�[�x�e�X�g�F�I��		�[�x���C�g�F�I�t
	depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[1][0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//	�[�x�e�X�g�F�I�t		�[�x���C�g�F�I�t
	depthStencilDesc = {};
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[0][0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	// --- �}�X�N�p ---
	
	// MODE::NONE
	depthStencilDesc = {};
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.StencilEnable = FALSE;
	hr = device->CreateDepthStencilState(&depthStencilDesc,
		maskDepthStencilStates[0/*NONE*/].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// MODE::MASK
	depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;//�[�x�e�X�g���s��
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;//�[�x�l���������܂Ȃ�
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;//�[�x�e�X�g�ɕK�����s����i�`��͂��Ȃ����X�e���V���l�͏������ށj
	depthStencilDesc.StencilEnable = TRUE;//�X�e���V���e�X�g���s��
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;//�X�e���V���e�X�g�ɂ͕K�����i
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_REPLACE;//�[�x�e�X�g�Ɏ��s���ăX�e���V���e�X�g�ɐ��������ꍇ
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	hr = device->CreateDepthStencilState(&depthStencilDesc,
		maskDepthStencilStates[1/*MASK*/].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// MODE::APPLY_MASK
	depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;//Turn off writes to the depth-stencil buffer.
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;//Never pass the comparison.
	depthStencilDesc.StencilEnable = TRUE;
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NOT_EQUAL;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	hr = device->CreateDepthStencilState(&depthStencilDesc,
		maskDepthStencilStates[2/*APPLY_MASK*/].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// MODE::EXCLUSIVE
	depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;//Turn off writes to the depth-stencil buffer.
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;//Never pass the comparison.
	depthStencilDesc.StencilEnable = TRUE;
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_EQUAL;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
	hr = device->CreateDepthStencilState(&depthStencilDesc,
		maskDepthStencilStates[3/*EXCLUSIVE*/].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


// --- �u�����f�B���O�X�e�[�g�̐��� ---
void Graphics::createBlendState()
{
	HRESULT hr{ S_OK };

	D3D11_BLEND_DESC blendDesc{};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blendDesc, blendStates[0/*NONE*/].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blendDesc, blendStates[1/*ALPHA*/].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blendDesc, blendStates[2/*ADD*/].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_SUBTRACT;
	//blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
	//blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
	//blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blendDesc, blendStates[3/*SUB*/].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


// --- Direct2D�֘A�̃I�u�W�F�N�g�̐��� ---
void Graphics::createDirect2DObjects()
{
	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<IDXGIDevice2> dxgiDevice2;
	{
		hr = device.As(&dxgiDevice2);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


	Microsoft::WRL::ComPtr<ID2D1Factory1> d2dFactory1;
	D2D1_FACTORY_OPTIONS factoryOptions{};
	{
#ifdef _DEBUG
		factoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, factoryOptions, d2dFactory1.GetAddressOf());
	}


	Microsoft::WRL::ComPtr<ID2D1Device> d2dDevice;
	{
		hr = d2dFactory1->CreateDevice(dxgiDevice2.Get(), d2dDevice.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, d2d1DeviceContext_.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = dxgiDevice2->SetMaximumFrameLatency(1);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


	Microsoft::WRL::ComPtr<IDXGISurface2> dxgiSurface2;
	{
		hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(dxgiSurface2.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


	Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2dBitmap1;
	{
		hr = d2d1DeviceContext_->CreateBitmapFromDxgiSurface(
			dxgiSurface2.Get(),
			D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)
			),
			d2dBitmap1.GetAddressOf()
		);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	
	d2d1DeviceContext_->SetTarget(d2dBitmap1.Get());
}


// --- �����_�[�^�[�Q�b�g�r���[�̃N���A ---
void Graphics::clearRenderTargetView(const DirectX::XMFLOAT4& color)
{
	immediateContext.Get()->ClearRenderTargetView(renderTargetView.Get(), (const float*)&color);
}


// --- �f�v�X�X�e���V���r���[�̃N���A ---
void Graphics::clearDepthStencilView()
{
	immediateContext.Get()->ClearDepthStencilView(
		depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0
	);
}



// --- ���X�^���C�U�X�e�[�g�I�u�W�F�N�g�̐��� ---
void Graphics::createRasterizerState()
{
	HRESULT hr{ S_OK };

	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;


	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[0/*CULL_NONE*/].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[1/*SOLID*/].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[2/*WIREFRAME*/].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[3/*WIREFRAME_CULL_NONE*/].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}


// --- �A�_�v�^�[�̐��� ---
void Graphics::createAdapter(IDXGIFactory6* dxgiFactory6)
{
	//Microsoft::WRL::ComPtr<IDXGIFactory> factory;
	//CreateDXGIFactory(IID_PPV_ARGS(&factory));
	//for (UINT adapterIndex = 0; S_OK == factory->EnumAdapters(adapterIndex, adapter_.GetAddressOf()); adapterIndex++) {

	//	DXGI_ADAPTER_DESC adapterDesc;
	//	adapter_.Get()->GetDesc(&adapterDesc);
	//	if (adapterDesc.VendorId == 0x1002/*AMD*/ ||
	//		adapterDesc.VendorId == 0x10DE/*NVIDIA*/) {

	//		break;
	//	}
	//}

	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<IDXGIAdapter3> enumeratedAdapter;

	for (UINT adapterIndex = 0;
		DXGI_ERROR_NOT_FOUND !=
		dxgiFactory6->EnumAdapterByGpuPreference(
			adapterIndex,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(enumeratedAdapter.ReleaseAndGetAddressOf()));
		++adapterIndex
		)
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		hr = enumeratedAdapter->GetDesc1(&adapterDesc);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		if (adapterDesc.VendorId == 0x1002/*AMD*/ || adapterDesc.VendorId == 0x10DE/*NVIDIA*/)
		{
			break;
		}
	}

	adapter_ = enumeratedAdapter.Detach();
}


// --- ���X�^���C�U�X�e�[�g�̐ݒ� ---
void Graphics::setRasterizeState(int state)
{
	immediateContext->RSSetState(rasterizerStates[state].Get());
}


// --- �u�����h�X�e�[�g�̐ݒ� ---
void Graphics::setBlendState(int state)
{
	immediateContext->OMSetBlendState(blendStates[state].Get(), nullptr, 0xFFFFFFFF);
}


// --- �[�x�X�e���V���̐ݒ� ---
void Graphics::setDepthStencil(int state)
{
	immediateContext->OMSetDepthStencilState(maskDepthStencilStates[state].Get(), 1);
}

void Graphics::setDepthStencil(bool test, bool write)
{
	immediateContext->OMSetDepthStencilState(depthStencilStates[static_cast<size_t>(test)][static_cast<size_t>(write)].Get(), 1);
}


// --- �T���v���[�X�e�[�g�̐ݒ� ---
void Graphics::setSamplerState()
{
	immediateContext->PSSetSamplers(0, 1, samplerStates[0].GetAddressOf());
	immediateContext->PSSetSamplers(1, 1, samplerStates[1].GetAddressOf());
	immediateContext->PSSetSamplers(2, 1, samplerStates[2].GetAddressOf());
	immediateContext->PSSetSamplers(3, 1, samplerStates[3].GetAddressOf());
}


// --- �����_�[�^�[�Q�b�g�̐ݒ� ---
void Graphics::setRenderTarget()
{
	immediateContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
}
