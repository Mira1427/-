#include "Particle.hlsli"

StructuredBuffer<Particle> particleBuffer : register(t0);

[maxvertexcount(4)]
void main(point VSOutput input[1] : SV_POSITION, inout TriangleStream<GSOutput> output)
{
	const float2 corners[] =
	{
		float2(-1.0f, -1.0f),
		float2(-1.0f, +1.0f),
		float2(+1.0f, -1.0f),
		float2(+1.0f, +1.0f)
	};

	const float2 texcoords[] =
	{
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
	};

	Particle p = particleBuffer[input[0].vertexID_];

	const float aspectRatio = 1920.0f / 1080.0f;
	float2 scale = float2(particle_.size_, particle_.size_ * aspectRatio);

	[unroll]
	for (uint vertexIndex = 0; vertexIndex < 4; vertexIndex++)
	{
		GSOutput element;

		element.position_ = mul(float4(p.position_, 1), scene_.viewProjection_);
		element.position_.xy += corners[vertexIndex] * scale;

		element.color_ = p.color_;
		element.texcoord_ = texcoords[vertexIndex];
		output.Append(element);
	}

	output.RestartStrip();
}