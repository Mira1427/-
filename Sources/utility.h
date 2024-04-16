#pragma once

#include <DirectXMath.h>

inline const float toRadians(const float& degrees) {

	return degrees * (DirectX::XM_PI / 180.0f);
}