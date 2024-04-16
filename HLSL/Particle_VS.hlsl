#include "Particle.hlsli"

VSOutput main( uint vertexID : SV_VERTEXID )
{
	VSOutput vout;
	vout.vertexID_ = vertexID;

	return vout;
}