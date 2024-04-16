#include "./SpriteRenderer.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState samplerStates[3] : register(s0);
Texture2D mainTexture : register(t0);


float4 main(VSOutput vout) : SV_TARGET
{
	float4 color = mainTexture.Sample(samplerStates[ANISOTROPIC], vout.texcoord_);

	return color * vout.color_;
}