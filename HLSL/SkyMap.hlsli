#include "./Constant.hlsli"

struct VSOutput
{
	float4 position_ : SV_POSITION;
	float2 texcoord_ : TEXCOORD;
};


cbuffer CBScene : register(b0)
{
	SceneConstants scene_;
}