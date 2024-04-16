#include "ShaderToy.hlsli"
#include "Constant.hlsli"

#define LABEL 1

#if LABEL == 0
//////////////////////////////////
//
//	 GLOWING WAVES
//
//   by Tech_
//
//////////////////////////////////

////////////////////////////////

#define GLOW_AMOUNT 1.3
#define SPEED 0.8

//////////////////////////////////

float3 getWave(in float2 uv, float curve, float3 color)
{
	uv.x += curve * 1.6;
	float wave = pow(1. - distance(uv.x, 0.5), 38.0 / GLOW_AMOUNT);
	return wave * color;
}

void mainImage(out float4 fragColor, in float2 fragCoord)
{
	// Normalized pixel coordinates (from 0 to 1)
	float2 uv = fragCoord / iResolution.xy;
	uv /= 2.0;
	uv.x += 0.25;

	float speed = iTime * SPEED;

	float3 red = float3(1.0, 0.0, 0.0);
	float3 blue = float3(0.0, 0.0, 1.0);
	float3 yellow = float3(1.0, 0.8, 0.0);
	float3 purple = float3(0.5, 0.0, 1.0);
	float3 orange = float3(1.0, 0.5, 0.0);

	// Time varying pixel color
	float3 col = 0;
	col += getWave(uv, sin((uv.y + speed * 0.05) * 10.) * 0.06, red);
	col += getWave(uv, cos((uv.y + speed * 0.025) * 10.) * 0.12, blue);
	col += getWave(uv, sin((uv.y + speed * -0.007 + cos(uv.y * 0.2)) * 8.) * 0.1, yellow);
	col += getWave(uv, cos((uv.y + speed * 0.035 + sin(uv.y * 0.13)) * 8.) * 0.05, purple);
	col += getWave(uv, sin((uv.y / 2.0 + speed * -0.05) * 10.) * 0.05, orange);

    col = mul(col, 0.8);

	// Output to screen
	fragColor = float4(col, 1.0);
}
#endif


#if LABEL == 1

#define DRAG_MULT 0.38 // changes how much waves pull on the water
#define WATER_DEPTH 1.0 // how deep is the water
#define CAMERA_HEIGHT 1.5 // how high the camera should be
#define ITERATIONS_RAYMARCH 12 // waves iterations of raymarching
#define ITERATIONS_NORMAL 37 // waves iterations when calculating normals

#define NormalizedMouse (iMouse.xy / iResolution.xy) // normalize mouse coords

// Calculates wave value and its derivative, 
// for the wave direction, position in space, wave frequency and time
float2 wavedx(float2 position, float2 direction, float frequency, float timeshift) {
    float x = dot(direction, position) * frequency + timeshift;
    float wave = exp(sin(x) - 1.0);
    float dx = wave * cos(x);
    return float2(wave, -dx);
}

// Calculates waves by summing octaves of various waves with various parameters
float getwaves(float2 position, int iterations) {
    float iter = 0.0; // this will help generating well distributed wave directions
    float frequency = 1.0; // frequency of the wave, this will change every iteration
    float timeMultiplier = 2.0; // time multiplier for the wave, this will change every iteration
    float weight = 1.0;// weight in final sum for the wave, this will change every iteration
    float sumOfValues = 0.0; // will store final sum of values
    float sumOfWeights = 0.0; // will store final sum of weights
    for (int i = 0; i < iterations; i++) {
        // generate some wave direction that looks kind of random
        float2 p = float2(sin(iter), cos(iter));
        // calculate wave data
        float2 res = wavedx(position, p, frequency, iTime * timeMultiplier);

        // shift position around according to wave drag and derivative of the wave
        position += p * res.y * weight * DRAG_MULT;

        // add the results to sums
        sumOfValues += res.x * weight;
        sumOfWeights += weight;

        // modify next octave ;
        weight = lerp(weight, 0.0, 0.2);
        frequency *= 1.18;
        timeMultiplier *= 1.07;

        // add some kind of random value to make next wave look random too
        iter += 1232.399963;
    }
    // calculate and return
    return sumOfValues / sumOfWeights;
}

// Raymarches the ray from top water layer boundary to low water layer boundary
float raymarchwater(float3 camera, float3 start, float3 end, float depth) {
    float3 pos = start;
    float3 dir = normalize(end - start);
    for (int i = 0; i < 64; i++) {
        // the height is from 0 to -depth
        float height = getwaves(float2(pos.x, pos.z), ITERATIONS_RAYMARCH) * depth - depth;
        // if the waves height almost nearly matches the ray height, assume its a hit and return the hit distance
        if (height + 0.01 > pos.y) {
            return distance(pos, camera);
        }
        // iterate forwards according to the height mismatch
        pos += dir * (pos.y - height);
    }
    // if hit was not registered, just assume hit the top layer, 
    // this makes the raymarching faster and looks better at higher distances
    return distance(start, camera);
}

// Calculate normal at point by calculating the height at the pos and 2 additional points very close to pos
float3 normal(float2 pos, float e, float depth) {
    float2 ex = float2(e, 0);
    float H = getwaves(pos, ITERATIONS_NORMAL) * depth;
    float3 a = float3(pos.x, H, pos.y);
    return normalize(
        cross(
            a - float3(pos.x - e, getwaves(pos - ex, ITERATIONS_NORMAL) * depth, pos.y),
            a - float3(pos.x, getwaves(pos + ex, ITERATIONS_NORMAL) * depth, pos.y + e)
        )
    );
}

// Helper function generating a rotation matrix around the axis by the angle
float3x3 createRotationMatrixAxisAngle(float3 axis, float angle) {
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    return float3x3(
        oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s,
        oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s,
        oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c
        );
}

// Helper function that generates camera ray based on UV and mouse
float3 getRay(float2 fragCoord) {
    float2 uv = ((fragCoord.xy / iResolution.xy) * 2.0 - 1.0) * float2(iResolution.x / iResolution.y, 1.0);
    // for fisheye, uncomment following line and comment the next one
    //float3 proj = normalize(float3(uv.x, uv.y, 1.0) + float3(uv.x, uv.y, -1.0) * pow(length(uv), 2.0) * 0.05);  
    float3 proj = normalize(float3(uv.x, uv.y, 1.5));
    if (iResolution.x < 600.0) {
        return proj;
    }
    return mul(createRotationMatrixAxisAngle(float3(0.0, -1.0, 0.0), 3.0 * ((NormalizedMouse.x + 0.5) * 2.0 - 1.0)),
        mul(createRotationMatrixAxisAngle(float3(1.0, 0.0, 0.0), 0.5 + 1.5 * ((NormalizedMouse.y * 1.5) * 2.0 - 1.0)), proj));
}

// Ray-Plane intersection checker
float intersectPlane(float3 origin, float3 direction, float3 point_, float3 normal) {
    return clamp(dot(point_ - origin, normal) / dot(direction, normal), -1.0, 9991999.0);
}

// Some very barebones but fast atmosphere approximation
float3 extra_cheap_atmosphere(float3 raydir, float3 sundir) {
    sundir.y = max(sundir.y, -0.07);
    float special_trick = 1.0 / (raydir.y * 1.0 + 0.1);
    float special_trick2 = 1.0 / (sundir.y * 11.0 + 1.0);
    float raysundt = pow(abs(dot(sundir, raydir)), 2.0);
    float sundt = pow(max(0.0, dot(sundir, raydir)), 8.0);
    float mymie = sundt * special_trick * 0.2;
    float3 suncolor = lerp(float3(1.0f, 1.0f, 1.0f), max(float3(0.0f, 0.0f, 0.0f), float3(1.0f, 1.0f, 1.0f) - float3(5.5, 13.0, 22.4) / 22.4), special_trick2);
    float3 bluesky = float3(5.5, 13.0, 22.4) / 22.4 * suncolor;
    float3 bluesky2 = max(float3(0.0f, 0.0f, 0.0f), bluesky - float3(5.5, 13.0, 22.4) * 0.002 * (special_trick + -6.0 * sundir.y * sundir.y));
    bluesky2 *= special_trick * (0.24 + raysundt * 0.24);
    return bluesky2 * (1.0 + 1.0 * pow(1.0 - raydir.y, 3.0));
}

// Calculate where the sun should be, it will be moving around the sky
float3 getSunDirection() {
    return normalize(float3(sin(iTime * 0.1), 1.0, cos(iTime * 0.1)));
}

// Get atmosphere color for given direction
float3 getAtmosphere(float3 dir) {
    return extra_cheap_atmosphere(dir, getSunDirection()) * 0.5;
}

// Get sun color for given direction
float getSun(float3 dir) {
    return pow(max(0.0, dot(dir, getSunDirection())), 720.0) * 210.0;
}

// Great tonemapping function from my other shader: https://www.shadertoy.com/view/XsGfWV
float3 aces_tonemap(float3 color) {
    float3x3 m1 = float3x3(
        0.59719, 0.07600, 0.02840,
        0.35458, 0.90834, 0.13383,
        0.04823, 0.01566, 0.83777
        );
    float3x3 m2 = float3x3(
        1.60475, -0.10208, -0.00327,
        -0.53108, 1.10813, -0.07276,
        -0.07367, -0.00605, 1.07602
        );
    float3 v = mul(m1, color);
    float3 a = v * (v + float3(0.0245786, 0.0245786, 0.0245786)) - float3(0.000090537, 0.000090537, 0.000090537);
    float3 b = v * (float3(0.983729, 0.983729, 0.983729) * v + float3(0.4329510, 0.4329510, 0.4329510)) + float3(0.238081, 0.238081, 0.238081);
    return pow(clamp(mul(m2, a / b), 0.0, 1.0), 1.0 / 2.2);
}

// Main
void mainImage(out float4 fragColor, in float2 fragCoord) {
    // get the ray
    float3 ray = getRay(fragCoord);
    if (ray.y >= 0.0) {
        // if ray.y is positive, render the sky
        float3 C = getAtmosphere(ray) + getSun(ray);
        fragColor = float4(aces_tonemap(C * 2.0), 1.0);
        return;
    }

    // now ray.y must be negative, water must be hit
    // define water planes
    float3 waterPlaneHigh = float3(0.0, 0.0, 0.0);
    float3 waterPlaneLow = float3(0.0, -WATER_DEPTH, 0.0);

    // define ray origin, moving around
    float3 origin = float3(iTime * 0.2, CAMERA_HEIGHT, 1);

    // calculate intersections and reconstruct positions
    float highPlaneHit = intersectPlane(origin, ray, waterPlaneHigh, float3(0.0, 1.0, 0.0));
    float lowPlaneHit = intersectPlane(origin, ray, waterPlaneLow, float3(0.0, 1.0, 0.0));
    float3 highHitPos = origin + ray * highPlaneHit;
    float3 lowHitPos = origin + ray * lowPlaneHit;

    // raymatch water and reconstruct the hit pos
    float dist = raymarchwater(origin, highHitPos, lowHitPos, WATER_DEPTH);
    float3 waterHitPos = origin + ray * dist;

    // calculate normal at the hit position
    float3 N = normal(float2(waterHitPos.x, waterHitPos.z), 0.01, WATER_DEPTH);

    // smooth the normal with distance to avoid disturbing high frequency noise
    N = lerp(N, float3(0.0, 1.0, 0.0), 0.8 * min(1.0, sqrt(dist * 0.01) * 1.1));

    // calculate fresnel coefficient
    float fresnel = (0.04 + (1.0 - 0.04) * (pow(1.0 - max(0.0, dot(-N, ray)), 5.0)));

    // reflect the ray and make sure it bounces up
    float3 R = normalize(reflect(ray, N));
    R.y = abs(R.y);

    // calculate the reflection and approximate subsurface scattering
    float3 reflection = getAtmosphere(R) + getSun(R);
    float3 scattering = float3(0.0293, 0.0698, 0.1717) * 0.1 * (0.2 + (waterHitPos.y + WATER_DEPTH) / WATER_DEPTH);

    // return the combined result
    float3 C = fresnel * reflection + scattering;
    fragColor = float4(aces_tonemap(C * 2.0), 1.0);
}
#endif

#if LABEL == 2
const float cloudscale = 1.1;
const float speed = 0.03;
const float clouddark = 0.5;
const float cloudlight = 0.3;
const float cloudcover = 0.2;
const float cloudalpha = 8.0;
const float skytint = 0.5;
const float3 skycolour1 = float3(0.2, 0.4, 0.6);
const float3 skycolour2 = float3(0.4, 0.7, 1.0);

const float2x2 m = float2x2(1.6, 1.2, -1.2, 1.6);

float2 hash(float2 p) {
    p = float2(dot(p, float2(127.1, 311.7)), dot(p, float2(269.5, 183.3)));
    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

float noise(in float2 p) {
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;
    float2 i = floor(p + (p.x + p.y) * K1);
    float2 a = p - i + (i.x + i.y) * K2;
    float2 o = (a.x > a.y) ? float2(1.0, 0.0) : float2(0.0, 1.0); //float2 of = 0.5 + 0.5*float2(sign(a.x-a.y), sign(a.y-a.x));
    float2 b = a - o + K2;
    float2 c = a - 1.0 + 2.0 * K2;
    float3 h = max(0.5 - float3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
    float3 n = h * h * h * h * float3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));
    return dot(n, float3(70.0, 70.0, 70.0));
}

float fbm(float2 n) {
    float total = 0.0, amplitude = 0.1;
    for (int i = 0; i < 7; i++) {
        total += noise(n) * amplitude;
        n = mul(m, n);
        amplitude = mul(amplitude, 0.4);
    }
    return total;
}

// -----------------------------------------------

void mainImage(out float4 fragColor, in float2 fragCoord) {
    float2 p = fragCoord.xy / iResolution.xy;
    float2 uv = p * float2(iResolution.x / iResolution.y, 1.0);
    float time = iTime * speed;
    float q = fbm(uv * cloudscale * 0.5);

    //ridged noise shape
    float r = 0.0;
    uv = mul(uv, cloudscale);
    uv -= q - time;
    float weight = 0.8;
    for (int i = 0; i < 8; i++) {
        r += abs(weight * noise(uv));
        uv = mul(m, uv) + time;
        weight = mul(weight, 0.7);
    }

    //noise shape
    float f = 0.0;
    uv = p * float2(iResolution.x / iResolution.y, 1.0);
    uv = mul(uv, cloudscale);
    uv -= q - time;
    weight = 0.7;
    for (int i = 0; i < 8; i++) {
        f += weight * noise(uv);
        uv = mul(m, uv) + time;
        weight = mul(weight, 0.6);
    }

    f = mul(f, r + f);

    //noise colour
    float c = 0.0;
    time = iTime * speed * 2.0;
    uv = p * float2(iResolution.x / iResolution.y, 1.0);
    uv = mul(uv, cloudscale * 2.0);
    uv -= q - time;
    weight = 0.4;
    for (int i = 0; i < 7; i++) {
        c += weight * noise(uv);
        uv = mul(m, uv) + time;
        weight = mul(weight, 0.6);
    }

    //noise ridge colour
    float c1 = 0.0;
    time = iTime * speed * 3.0;
    uv = p * float2(iResolution.x / iResolution.y, 1.0);
    uv = mul(uv, cloudscale * 3.0);
    uv -= q - time;
    weight = 0.4;
    for (int i = 0; i < 7; i++) {
        c1 += abs(weight * noise(uv));
        uv = mul(m, uv) + time;
        weight = mul(weight, 0.6);
    }

    c += c1;

    float3 skycolour = mix(skycolour2, skycolour1, p.y);
    float3 cloudcolour = float3(1.1, 1.1, 0.9) * clamp((clouddark + cloudlight * c), 0.0, 1.0);

    f = cloudcover + cloudalpha * f * r;

    float3 result = mix(skycolour, clamp(skytint * skycolour + cloudcolour, 0.0, 1.0), clamp(f + c, 0.0, 1.0));

    fragColor = float4(result, 1.0);
}
#endif



float4 main(VS_OUT pin) : SV_TARGET{

	float4 fragColor = 0;
	float2 fragCoord = pin.position_.xy;

	mainImage(fragColor, fragCoord);

	return fragColor;
}