#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>

class FrameBuffer {
public:
	FrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height);
	virtual ~FrameBuffer() = default;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[2];
	D3D11_VIEWPORT viewport;

public:
	void clear(ID3D11DeviceContext* context,
		float r = 0, float g = 0, float b = 0, float a = 0, float depth = 1);

	void active(ID3D11DeviceContext* context);
	void deactive(ID3D11DeviceContext* context);

private:
	UINT viewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cahedDepthStencilView;
};

