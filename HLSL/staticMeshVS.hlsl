#include "staticMesh.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD) {

	VS_OUT vout;
	vout.position = mul(position, mul(object_.world_, scene_.viewProjection_));

	vout.worldPosition = mul(position, object_.world_);
	normal.w = 0;
	vout.worldNormal = normalize(mul(normal, object_.world_));

	vout.color = object_.color_;
	vout.texcoord = texcoord;

	return vout;
}