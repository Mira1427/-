#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

#include "../Library/Vector.h"

class ShaderToy {
public:
	ShaderToy(ID3D11Device* device);
	virtual ~ShaderToy() = default;

	void blit(ID3D11DeviceContext* immediateContext, float time,
		ID3D11ShaderResourceView** shaderResourceView, uint32_t startSlot, uint32_t numViews,
		ID3D11PixelShader* replacedPixelShader = nullptr);

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> embeddedVertexShader_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> embeddedPixelShader_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer_;

	struct Constants {

		DirectX::XMFLOAT4 iMouse_;
		DirectX::XMFLOAT4 iChannelResolution[4];
		Vector2 iResolution_;
		float iTime_;
		float iFrame_;
	};
};

