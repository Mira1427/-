#include "ShaderToy.hlsli"

VS_OUT main(in uint vertexID : SV_VERTEXID){

	VS_OUT vout;

	const float2 position[4] = {
		{ -1, +1 },
		{ +1, +1 },
		{ -1, -1 },
		{ +1, -1 }
	};

	const float2 texcoords[4] = {
		{ 0, 0 },
		{ 1, 0 },
		{ 0, 1 },
		{ 1, 1 }
	};

	vout.position_ = float4(position[vertexID], 0, 1);
	vout.texcoord_ = texcoords[vertexID];

	return vout;
}