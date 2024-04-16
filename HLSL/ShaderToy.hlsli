struct VS_OUT {

	float4 position_ : SV_POSITION;
	float2 texcoord_ : TEXCOORD;
};


Texture2D iChannel0 : register(t0);
Texture2D iChannel1 : register(t1);
Texture2D iChannel2 : register(t2);
Texture2D iChannel3 : register(t3);

SamplerState linearClamp : register(s0);


float4 Sample(Texture2D iChannel, float2 texcoord) {
	
	return iChannel.Sample(linearClamp, texcoord);
}


float4 SampleLevel(Texture2D iChannel, float2 texcoord, float lod) {

	return iChannel.SampleLevel(linearClamp, texcoord, lod);
}


float4 Load(Texture2D iChannel, int2 location, int index) {

	return iChannel.Load(int3(location, 0), index);
}


typedef float2 vec2;
typedef float3 vec3;
typedef float4 vec4;

typedef int2 ivec2;
typedef int3 ivec3;
typedef int4 ivec4;

typedef uint2 uvec2;
typedef uint3 uvec3;
typedef uint4 uvec4;

typedef float2x2 mat2;
typedef float3x3 mat3;
typedef float4x4 mat4;

#define mix lerp
#define mod fmod
#define fract frac
#define atan atan2
#define inversesqrt rsqrt
#define texture Sample
#define textureLod SampleLevel
#define texelFetch Load
#define sampler2D Texture2D