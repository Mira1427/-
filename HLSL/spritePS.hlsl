#include "sprite.hlsli"

Texture2D colorMap : register(t0);
SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1);
SamplerState anisotropic_sampler_state : register(s2);

float4 main(VS_OUT pin) : SV_TARGET{

	float4 color = colorMap.Sample(anisotropic_sampler_state, pin.texcoord);
	float alpha = color.a;

#if 0
	//	�t�K���}�␳
	const float GAMMA = 2.2;
	color.rgb = pow(color.rgb, GAMMA);
#endif

	return float4(color.rgb, alpha) * pin.color;
}