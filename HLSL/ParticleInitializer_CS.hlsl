#include "Particle.hlsli"
#include "Common.hlsli"

RWStructuredBuffer<Particle> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main( uint3 dtID : SV_DispatchThreadID )
{
	uint id = dtID.x;

	Particle p = particleBuffer[id];

	const float noiseScale = 1.0f;
	float f0 = rand(id * noiseScale);
	float f1 = rand(f0 * noiseScale);
	float f2 = rand(f1 * noiseScale);

	p.position_ = particle_.emitterPosition_;

	p.velocity_.x = 0.5 * sin(2 * PI * f0);
	p.velocity_.y = 2.0 * f1;
	p.velocity_.z = 0.5 * cos(2 * PI * f0);

	p.color_.x = 1.0f;
	p.color_.y = f0 * 0.5f;
	p.color_.z = f0 * 0.05f;
	p.color_.xyz *= 3.0f;
	p.color_.w = 1.0f;

	p.age_ = 10.0 * f2;
	p.state_ = 0;

	particleBuffer[id] = p;
}