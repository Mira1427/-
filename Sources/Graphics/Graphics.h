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


// ===== グラフィックスクラス ==================================================================================================================================
class Graphics {
private:
	Graphics();

public:
	// --- インスタンスの取得 ---
	static Graphics& instance() {
		static Graphics instance_;
		return instance_;
	}

	void initialize();


	// --- DirectX11関連 ---
	ID3D11Device* getDevice() { return device.Get(); }
	ID3D11DeviceContext* getDeviceContext() { return immediateContext.Get(); }
	IDXGISwapChain1* getSwapChain() { return swapChain_.Get(); }
	ID3D11RenderTargetView* getRenderTargetView() { return renderTargetView.Get(); }
	ID3D11DepthStencilView* getDepthStencilView() { return depthStencilView.Get(); }
	IDXGIAdapter3* getAdapter() { return adapter_.Get(); }

	// --- Direct2D関連 ---
	ID2D1DeviceContext* getD2D1DeviceContext() { return d2d1DeviceContext_.Get(); }
	IDWriteTextFormat* getDWriteTextFormat(size_t index) { return dwriteTextFormats_[index].Get(); }
	ID2D1SolidColorBrush* getD2DSolidColorBrush(size_t index) { return d2dSolidColorBrushes_[index].Get(); }


	// ===== シェーダー関連 ==================================================================================================================================

	// --- 頂点シェーダーの作成 ---
	static void createVSFromCSO(ID3D11Device* device, const char* CSOName, ID3D11VertexShader** vertexShader);


	// --- 頂点シェーダーと入力レイアウトの作成 ---
	static void createVSAndILFromCSO(ID3D11Device* device, const char* CSOName,
		ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout,
		D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements);


	// --- ピクセルシェーダーの作成 ---
	static void createPSFromCSO(ID3D11Device* device, const char* CSOName, ID3D11PixelShader** pixelShader);


	// --- ジオメトリシェーダーの作成 ---
	static void createGSFromCSO(ID3D11Device* device, const char* CSOName, ID3D11GeometryShader** geometryShader);


	// --- コンピュートシェーダーの作成 ---
	static void createCSFromCSO(ID3D11Device* device, const char* CSOName, ID3D11ComputeShader** computeShader);


	// ===== バッファ関連 ==================================================================================================================================
	
	// --- 頂点バッファの作成 ---
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


	// --- インデックスバッファの作成 ---
	static void createIndexBuffer(ID3D11Device* device, ID3D11Buffer** ib, UINT byteSize, uint32_t* pData);


	// --- 定数バッファの作成 ---
	static void createConstantBuffer(ID3D11Device* device, ID3D11Buffer** cb, UINT byteSize);

	// --- 定数バッファの更新 ---
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


	// --- テクスチャの読み込み ---
	static void loadTextureFromFile(ID3D11Device* device, const wchar_t* fileName, ID3D11ShaderResourceView** srv, D3D11_TEXTURE2D_DESC* tex2DDesc);


	// --- 描画クラス関連 ---
	DebugRenderer* getDebugRenderer() const { return debugRenderer_.get(); }
	DebugLineRenderer* getDebugLineRenderer() const { return debugLineRenderer_.get(); }
	SpriteRenderer* getSpriteRenderer() const { return spriteRenderer_.get(); }
	Primitive* getPrimitiveRenderer() const { return primitiveRenderer_.get(); }

	
	// --- 管理クラス関連 ---
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
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates[2/*深度テスト*/][2/*深度書き込み*/];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> maskDepthStencilStates[4];
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendStates[4];
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffers[8];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[4];


private:
	// --- CSOファイルの読み込み ---
	static void loadCSOFile(const char* CSOName, std::unique_ptr<unsigned char[]>& CSOData, long& CSOSize);

	// --- 頂点シェーダーの作成 ---
	static void createVS(ID3D11Device* device, const unsigned char* CSOData, size_t CSOSize, ID3D11VertexShader** vertexShader);

	// --- ピクセルシェーダーの作成 ---
	static void createPS(ID3D11Device* device, const unsigned char* CSOData, size_t CSOSize, ID3D11PixelShader** pixelShader);

	// --- ジオメトリシェーダーの作成 ---
	static void createGS(ID3D11Device* device, const unsigned char* CSOData, size_t CSOSize, ID3D11GeometryShader** geometryShader);

	// --- コンピュートシェーダーの作成 ---
	static void createCS(ID3D11Device* device, const unsigned char* CSOData, size_t CSOSize, ID3D11ComputeShader** computeShader);

	// --- 入力レイアウトの作成 ---
	static void createIL(ID3D11Device* device, const D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements, const unsigned char* CSOData, size_t CSOSize, ID3D11InputLayout** inputLayout);


	// --- DirectX11関連 ---
	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain1>			swapChain_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;
	Microsoft::WRL::ComPtr<IDXGIAdapter3> adapter_;

	// --- Direct2D関連 ---
	Microsoft::WRL::ComPtr<ID2D1DeviceContext>		d2d1DeviceContext_;
	Microsoft::WRL::ComPtr<IDWriteTextFormat>		dwriteTextFormats_[8];
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>	d2dSolidColorBrushes_[8];

	// --- レンダラー関連 ---
	std::unique_ptr<DebugRenderer>		debugRenderer_;
	std::unique_ptr<DebugLineRenderer>	debugLineRenderer_;
	std::unique_ptr<SpriteRenderer>		spriteRenderer_;
	std::unique_ptr<Primitive>			primitiveRenderer_;

	// --- 管理クラス関連 ---
	std::unique_ptr<IlluminationManager> illumination_;
};


enum class BlendState { NONE, ALPHA, ADD };

enum class RasterState { CULL_NONE, SOLID, WIREFRAME, WIREFRAME_CULL_NONE };


// --- 定数バッファ構造体 ---
struct Constants {

	// --- 定数のラベル ---
	enum { OBJECT, SCENE, MATERIAL, DISSOLVE, WINDOW, SHADER_TOY, };

	// --- オブジェクト定数 ---
	struct Object
	{
		Matrix world_;		// ワールド座標
		Vector4	color_;					// 色

	};

	// --- シーン定数 ---
	struct Scene
	{
		Matrix	viewProjection_;	//	ビュー・プロジェクション変換行列
		Matrix invViewProjection_;	//	ビュープロジェクションの逆行列
		Vector3	cameraPosition_;	//	カメラの位置
	};


	// --- マテリアル定数 ---
	struct Material
	{
		float specular_{ 128 };	//	スペキュラー
	};


	// --- ディゾルブ定数 ---
	struct Dissolve
	{
		float amount_;
	};

	// --- ウィンドウ定数 ---
	struct Window
	{
		float width_;
		float height_;
	};

	// --- シェーダートイ定数 ---
	struct ShaderToy
	{
		Vector4 iMouse_;
		Vector4 iChannelResolution_[4];
		Vector4 iResolution_;
		float iTime_;
		float iFrame_;
	};
};