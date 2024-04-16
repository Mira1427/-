#include "./SkyMap.hlsli"

SamplerState samplerStates[3] : register(s0);
Texture2D skyMap : register(t0);

float4 main(VSOutput pin) : SV_TARGET
{
	float4 R = mul(
		float4(
			(pin.texcoord_.x * 2.0) - 1.0,
			1.0 - (pin.texcoord_.y * 2.0),
			1, 1),
		scene_.invViewProjection_
	);

	R /= R.w;
	float3 v = normalize(R.xyz);


	// --- Blinn / Newell の緯度マッピング ---
	float2 samplerPoint;
	samplerPoint.x = (atan2(v.z, v.x) + PI) / (PI * 2.0);
	samplerPoint.y = 1.0 - ((asin(v.y) + PI * 0.5) / PI);


	// --- lod は Level of Detail ( 解像度 ) ---
	//     高 0.0 --- 1.0 低
	const float lod = 0.0;
	return skyMap.SampleLevel(samplerStates[LINEAR], samplerPoint, lod);
}