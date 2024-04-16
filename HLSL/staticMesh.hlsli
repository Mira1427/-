#include "./Constant.hlsli"
#include "./Common.hlsli"

struct VS_OUT
{
	float4 position			: SV_POSITION;
	float4 color			: COLOR;
	float2 texcoord			: TEXCOORD;
	float4 worldPosition	: POSITION;
	float4 worldNormal		: NORMAL;
};


// --- �I�u�W�F�N�g�萔 ---
struct ObjectConstants
{
	row_major float4x4	world_;
	float4				color_;
};

cbuffer CBObject : register(b0)
{
	ObjectConstants object_;
}


// --- �V�[���萔 ---
cbuffer CBScene : register(b1)
{
	SceneConstants scene_;
}


// --- �Ɩ��萔 ---
cbuffer CBIllumination : register(b2)
{
	AmbientLight	ambientLight_;
	DirectionLight	directionLight_;
	PointLight		pointLights_[POINT_LIGHT_MAX];
}
