#include "./Constant.hlsli"

struct VSInput {

	float4 position : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 texcoord : TEXCOORD;
	float4 boneWeights : WEIGHTS;
	uint4 boneIndices : BONES;
};


struct VSOutput {

	float4 position : SV_POSITION;
	float4 worldPosition : POSITION;
	float4 worldNormal : NORMAL;
	float4 worldTangent : TANGENT;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};


static const int MAxBONES = 512;


// --- オブジェクト定数 ---
struct ObjectConstants
{
	row_major float4x4 world_;
	float4 color_;
	row_major float4x4 boneTransforms_[MAxBONES];
};


// --- オブジェクト定数バッファ ---
cbuffer CBObject : register(b0)
{
	ObjectConstants object_;
}


// --- シーン定数バッファ ---
cbuffer CBScene : register(b1)
{
	SceneConstants scene_;
}


// --- 照明定数バッファ ---
cbuffer CBIllumination : register(b2)
{
	AmbientLight	ambientLight_;
	DirectionLight	directionLight_;
	PointLight		pointLights_[POINT_LIGHT_MAX];
}
