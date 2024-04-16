#pragma once

#include <string>

#include <d3d11.h>
#include <wrl.h>

#include "../Library/Matrix.h"


// ===== テクスチャ構造体 ==================================================================================================================================
struct Texture
{
	Texture() {};
	Texture(ID3D11Device* device, const char* fileName);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture_;
	Vector2 size_;
	const char* fileName_;
};


// ===== スプライト描画クラス =============================================================================================================================
class SpriteRenderer
{
public:
	// --- コンストラクタ ---
	SpriteRenderer(ID3D11Device* device);

	// --- 描画処理 ---
	void draw(
		ID3D11DeviceContext* dc, Texture& texture,
		const Vector3& position,
		const Vector3& scale,
		const Vector2& texPos,
		const Vector2& texSize,
		const Vector2& center = Vector2::Zero_,
		const Vector3& rotation = Vector3::Zero_,
		const Vector4& color = Vector4::White_,
		bool inWorld = false,
		bool useBillboard = false
	);

	// --- 描画処理 ( ディゾルブ ) ---
	void drawDissolve(ID3D11DeviceContext* dc, Texture& texture, Texture& noiseTexture, float dissolveAmount, const Vector3& position, const Vector3& scale, const Vector2& texPos, const Vector2& texSize, const Vector2& center, const Vector3& rotation, const Vector4& color, bool inWorld);

	// --- 2Dオブジェクトの定数バッファ構造体 ---
	struct ObjectConstants2D
	{
		Vector2 position_;
		Vector2 size_;
		Vector4 color_;
		Vector2 center_;
		Vector2 scale_;
		Vector2 texPos_;
		Vector2 texSize_;
		float	rotation_;
	};

	// --- 3Dオブジェクトの定数バッファ構造体 ---
	struct ObjectConstants3D
	{
		Matrix  world_;
		Vector4 color_;
		Vector2 offset_;
		Vector2 size_;
		Vector2 texPos_;
		Vector2 texSize_;
	};

	// --- 頂点データ構造体 ---
	struct Vertex
	{
		Vector3 position_;
		Vector2 texcoord_;
	};

	// --- シェーダーのリソース構造体 ---
	struct ShaderResources
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>		vertexBuffer_;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader_;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		constantBuffer_;
	};

	enum
	{
		SPRITE2D,
		SPRITE3D,
	};

private:
	ShaderResources shaderResources_[2];
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	dissolvePixelShader_;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout_;
};

