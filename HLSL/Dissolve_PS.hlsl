#include "./SpriteRenderer.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState samplerStates[3] : register(s0);

Texture2D mainTexture : register(t0);
Texture2D noiseTexture : register(t1);

cbuffer CBDissolve : register(b3)
{
	DissolveConstants dissolve_;
}


float4 main(VSOutput vout) : SV_TARGET
{
	float4 color = mainTexture.Sample(samplerStates[ANISOTROPIC], vout.texcoord_);
	float4 noise = noiseTexture.Sample(samplerStates[ANISOTROPIC], vout.texcoord_);

	float alpha = floor(dissolve_.amount_ + min(0.999f, noise.r));
	color.a -= alpha;

	return color * vout.color_;
}