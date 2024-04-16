#include "ShaderToy.hlsli"
#include "Constant.hlsli"

#if 1

#define GLOW_AMOUNT 1.3
#define SPEED 0.8

vec3 getWave(in vec2 uv, float curve, vec3 color)
{
	uv.x += curve * 1.6;
	float wave = pow(1. - distance(uv.x, 0.5), 38.0 / GLOW_AMOUNT);
	return wave * color;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	// Normalized pixel coordinates (from 0 to 1)
	vec2 uv = fragCoord / iResolution.xy;
	uv /= 2.0;
	uv.x += 0.25;

	float speed = iTime * SPEED;

	vec3 red = vec3(1.0, 0.0, 0.0);
	vec3 blue = vec3(0.0, 0.0, 1.0);
	vec3 yellow = vec3(1.0, 0.8, 0.0);
	vec3 purple = vec3(0.5, 0.0, 1.0);
	vec3 orange = vec3(1.0, 0.5, 0.0);

	// Time varying pixel color
	vec3 col = 0;
	col += getWave(uv, sin((uv.y + speed * 0.05) * 10.) * 0.06, red);
	col += getWave(uv, cos((uv.y + speed * 0.025) * 10.) * 0.12, blue);
	col += getWave(uv, sin((uv.y + speed * -0.007 + cos(uv.y * 0.2)) * 8.) * 0.1, yellow);
	col += getWave(uv, cos((uv.y + speed * 0.035 + sin(uv.y * 0.13)) * 8.) * 0.05, purple);
	col += getWave(uv, sin((uv.y / 2.0 + speed * -0.05) * 10.) * 0.05, orange);

	col *= 0.8;

	// Output to screen
	fragColor = vec4(col, 1.0);
}

#endif

float4 main(VS_OUT pin) : SV_TARGET{

	float4 fragColor = 0;
	float2 fragCoord = pin.position_.xy;

	mainImage(fragColor, fragCoord);

	return fragColor;
}