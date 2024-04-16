#include "skinnedMesh.hlsli"

VSOutput main(VSInput vin) {

	vin.normal.w = 0;
	float sigma = vin.tangent.w;
	vin.tangent.w = 0;

	float4 blendedPosition = { 0, 0, 0, 1 };
	float4 blendedNormal = { 0, 0, 0, 0 };
	float4 blendedTangent = { 0, 0, 0, 0 };

	for (int boneIndex = 0; boneIndex < 4; boneIndex++) {

		blendedPosition +=
			vin.boneWeights[boneIndex] * mul(vin.position, object_.boneTransforms_[vin.boneIndices[boneIndex]]);

		blendedNormal +=
			vin.boneWeights[boneIndex] * mul(vin.normal, object_.boneTransforms_[vin.boneIndices[boneIndex]]);

		blendedTangent +=
			vin.boneWeights[boneIndex] * mul(vin.tangent, object_.boneTransforms_[vin.boneIndices[boneIndex]]);
	}

	vin.position = float4(blendedPosition.xyz, 1.0f);
	vin.normal = float4(blendedNormal.xyz, 0.0f);
	vin.tangent = float4(blendedTangent.xyz, 0.0f);


	VSOutput vout;
	vout.position = mul(vin.position, mul(object_.world_, scene_.viewProjection_));

	vout.worldPosition = mul(vin.position, object_.world_);
	vout.worldNormal = normalize(mul(vin.normal, object_.world_));
	vout.worldTangent = normalize(mul(vin.tangent, object_.world_));
	vout.worldTangent.w = sigma;

	vout.texcoord = vin.texcoord;

	vout.color = object_.color_;

	return vout;
}