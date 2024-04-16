#include "./Constant.hlsli"

struct VSInput
{
	float3 position_	: POSITION;
	float2 texcoord_	: TEXCOORD;
};

struct VSOutput
{
	float4 position_	: SV_POSITION;
	float4 color_		: COLOR;
	float2 texcoord_	: TEXCOORD;
};


// --- 2Dオブジェクト定数 ---
struct ObjectConstants2D
{
	float2	position_;
	float2	size_;
	float4	color_;
	float2	center_;
	float2	scale_;
	float2	texPos_;
	float2	texSize_;
	float	rotation_;
};

// --- 3Dオブジェクト定数 ---
struct ObjectConstants
{
	row_major float4x4	world_;
	float4				color_;
	float2				offset_;
	float2				size_;
	float2				texPos_;
	float2				texSize_;
};


// --- シーン定数 ---
cbuffer CBScene : register(b1)
{
	SceneConstants scene_;
}

// --- ウィンドウ定数 ---
cbuffer CBWindow : register(b2)
{
	WindowConstants window_;
}