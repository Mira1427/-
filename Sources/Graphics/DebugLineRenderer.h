#pragma once

#include <d3d11.h>
#include <wrl.h>

// ===== デバッグライン描画クラス ========================================================================================================================
class DebugLineRenderer
{
public:
	DebugLineRenderer(ID3D11Device* device, int lineNum);

	void draw(ID3D11DeviceContext* dc);

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader_;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		vertexBuffer_;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout_;
	UINT vertexCount_;
};

