#include "./Constant.hlsli"

// --- パーティクル定数バッファ ---
cbuffer CBParticle : register(b0)
{
	ParticleConstants particle_;
}


// --- シーン定数バッファ ---
cbuffer CBScene : register(b1)
{
	SceneConstants scene_;
}


// --- 頂点シェーダーの出力構造体 ---
struct VSOutput
{
	uint vertexID_ : VERTEXID;
};


// --- ジオメトリシェーダーの出力構造体 ---
struct GSOutput
{
	float4 position_ : SV_POSITION;
	float4 color_ : COLOR;
	float2 texcoord_ : TEXCOORD;
};


// --- パーティクルの構造体 ---
struct Particle
{
	float4	color_;
	float3	position_;
	float	age_;
	float3	velocity_;
	int		state_;
};

#define NUMTHREADS_X 16