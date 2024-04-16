#include "DebugRenderer.hlsli"

float4 main(VSOutput vout) : SV_TARGET
{
	return vout.color_;
}