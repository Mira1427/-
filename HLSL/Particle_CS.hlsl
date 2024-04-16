#include "Particle.hlsli"

RWStructuredBuffer<Particle> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main( uint3 dtID : SV_DispatchThreadID )
{
	uint id = dtID.x;

	Particle p = particleBuffer[id];

	if (p.age_ > 10.0f)
	{
		const float g = -0.5f;
		p.velocity_.y += g * particle_.deltaTime_;
		p.position_ += p.velocity_ * particle_.deltaTime_;

		if (p.position_.y < 0.0f)
		{
			p.velocity_ = 0.0f;
			p.position_ = 0.0f;
		}
	}

	p.age_ += particle_.deltaTime_;

	particleBuffer[id] = p;
}