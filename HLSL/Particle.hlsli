#include "./Constant.hlsli"

// --- �p�[�e�B�N���萔�o�b�t�@ ---
cbuffer CBParticle : register(b0)
{
	ParticleConstants particle_;
}


// --- �V�[���萔�o�b�t�@ ---
cbuffer CBScene : register(b1)
{
	SceneConstants scene_;
}


// --- ���_�V�F�[�_�[�̏o�͍\���� ---
struct VSOutput
{
	uint vertexID_ : VERTEXID;
};


// --- �W�I���g���V�F�[�_�[�̏o�͍\���� ---
struct GSOutput
{
	float4 position_ : SV_POSITION;
	float4 color_ : COLOR;
	float2 texcoord_ : TEXCOORD;
};


// --- �p�[�e�B�N���̍\���� ---
struct Particle
{
	float4	color_;
	float3	position_;
	float	age_;
	float3	velocity_;
	int		state_;
};

#define NUMTHREADS_X 16