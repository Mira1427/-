#pragma once

#include <DirectXMath.h>

class Math {

public:
	
	static float clamp(const float& v, const float& min, const float& max);
	static float clampMin(const float& v, const float& min);
	static float clampMax(const float& v, const float& max);
	
	static bool  isOutOfRange(const float& v, const float& min, const float& max);
	static bool  isOutOfRangeMin(const float& v, const float& min);
	static bool  isOutOfRangeMax(const float& v, const float& max);

	static int rand(const int& range, const int& offset);
	static float frand(const float& min, const float& max);

	static void normalize(DirectX::XMFLOAT2& v);

	static const float toRadians(const float& degrees);
};