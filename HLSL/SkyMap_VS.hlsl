#include "./SkyMap.hlsli"

VSOutput main( in uint vertexID : SV_VERTEXID )
{
	VSOutput vout;

	const float2 positions[4] =
	{
		{ -1.0f, +1.0f },
		{ +1.0f, +1.0f },
		{ -1.0f, -1.0f },
		{ +1.0f, -1.0f }
	};

	const float2 texcoords[4] =
	{
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 0.0f, 1.0f },
		{ 1.0f, 1.0f }
	};

	vout.position_ = float4(positions[vertexID], 0, 1);
	vout.texcoord_ = texcoords[vertexID];

	return vout;
}