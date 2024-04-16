#include "SkyMap.hlsli"

SamplerState samplerStates[3] : register(s0);
TextureCube skyBox : register(t0);

float4 main(VSOutput pin) : SV_TARGET
{
	float4 R = mul(
		float4(
			(pin.texcoord_.x * 2.0) - 1.0,
			1.0 - (pin.texcoord_.y * 2.0),
			1.0, 1.0),
		scene_.invViewProjection_
	);

	R /= R.w;

	const float lod = 0.0;
	return skyBox.SampleLevel(samplerStates[LINEAR], R.xyz, lod);
}