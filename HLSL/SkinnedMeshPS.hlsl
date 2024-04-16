#include "SkinnedMesh.hlsli"
#include "./Common.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState samplerStates[3] : register(s0);
Texture2D textureMaps[4] : register(t0);


float4 main(VSOutput pin) : SV_TARGET
{		
	// --- 法線マップから法線を取得 ---
	float3 N = normalize(pin.worldNormal.xyz);
	float3 T = normalize(pin.worldTangent.xyz);
	float sigma = pin.worldTangent.w;
	T = normalize(T - N * dot(N, T));
	float3 B = normalize(cross(N, T) * sigma);
	
	float4 normal = textureMaps[1].Sample(samplerStates[LINEAR], pin.texcoord);
	normal = (normal * 2.0) - 1.0;
	N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));


	// ===== ディレクションライトの処理 ========================================================================================================================

	// --- Lambert拡散反射光の計算 ---
	float3 diffDirection = calcLambertDiffuse(
		directionLight_.direction_,
		mul(directionLight_.color_, directionLight_.intensity_),
		N
	);

	// --- Phong鏡面反射光の計算 ---
	float3 specDirection = calcPhongSpecular(
		directionLight_.direction_,
		mul(directionLight_.color_, directionLight_.intensity_),
		scene_.cameraPosition_.xyz,
		pin.worldPosition.xyz,
		N
	);


	// ===== ポイントライトの処理 =============================================================================================================================

	float3 diffPoint = float3(0.0f, 0.0f, 0.0f);
	float3 specPoint = float3(0.0f, 0.0f, 0.0f);

	[unroll]
	for (int i = 0; i < POINT_LIGHT_MAX; i++)
	{
		float3 ligDir = pin.worldPosition - pointLights_[i].position_;
		ligDir = normalize(ligDir);

		float3 ligColor = mul(pointLights_[i].color_, pointLights_[i].intensity_);

		float3 diffuse = calcLambertDiffuse(
			ligDir,
			ligColor,
			N
		);

		float3 specular = calcPhongSpecular(
			ligDir,
			ligColor,
			scene_.cameraPosition_.xyz,
			pin.worldPosition.xyz,
			N
		);

		float distance = length(pin.worldPosition.xyz - pointLights_[i].position_);

		float affect = 1.0f - 1.0f / pointLights_[i].range_ * distance;

		affect = max(0.0f, affect);

		affect = pow(affect, 3.0f);

		diffuse *= affect;
		specular *= affect;

		diffPoint += diffuse;
		specPoint += specular;
	}

	float3 light = diffDirection + specDirection + diffPoint + specPoint + ambientLight_.color_;

	float4 finalColor = textureMaps[0].Sample(samplerStates[ANISOTROPIC], pin.texcoord);

	// --- 逆ガンマ補正 ---
	const float GAMMA = 2.2;
	finalColor.rgb = pow(finalColor.rgb, GAMMA);

	finalColor.xyz *= light;

	return finalColor * pin.color;
}