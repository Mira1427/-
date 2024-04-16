#pragma once

#include <string>

#include <d3d11.h>
#include <wrl.h>

#include "../Library/Matrix.h"


// ===== �e�N�X�`���\���� ==================================================================================================================================
struct Texture
{
	Texture() {};
	Texture(ID3D11Device* device, const char* fileName);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture_;
	Vector2 size_;
	const char* fileName_;
};


// ===== �X�v���C�g�`��N���X =============================================================================================================================
class SpriteRenderer
{
public:
	// --- �R���X�g���N�^ ---
	SpriteRenderer(ID3D11Device* device);

	// --- �`�揈�� ---
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

	// --- �`�揈�� ( �f�B�]���u ) ---
	void drawDissolve(ID3D11DeviceContext* dc, Texture& texture, Texture& noiseTexture, float dissolveAmount, const Vector3& position, const Vector3& scale, const Vector2& texPos, const Vector2& texSize, const Vector2& center, const Vector3& rotation, const Vector4& color, bool inWorld);

	// --- 2D�I�u�W�F�N�g�̒萔�o�b�t�@�\���� ---
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

	// --- 3D�I�u�W�F�N�g�̒萔�o�b�t�@�\���� ---
	struct ObjectConstants3D
	{
		Matrix  world_;
		Vector4 color_;
		Vector2 offset_;
		Vector2 size_;
		Vector2 texPos_;
		Vector2 texSize_;
	};

	// --- ���_�f�[�^�\���� ---
	struct Vertex
	{
		Vector3 position_;
		Vector2 texcoord_;
	};

	// --- �V�F�[�_�[�̃��\�[�X�\���� ---
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

