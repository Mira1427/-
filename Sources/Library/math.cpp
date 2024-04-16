#include "math.h"
#include <random>


//===============================
//			�N�����v
//===============================
float Math::clamp(const float& v, const float& min, const float& max) {

	if (v > max || v < min) {

		if (v > max) { return max; }
		if (v < min) { return min; }
	}

	return v;
}

float Math::clampMin(const float& v, const float& min) {

	if (v < min) { return min; }

	return v;
}

float Math::clampMax(const float& v, const float& max) {

	if (v > max) { return max; }

	return v;
}


bool Math::isOutOfRange(const float& v, const float& min, const float& max) {

	if (v > max || v < min) {

		return true;
	}

	return false;
}

bool Math::isOutOfRangeMin(const float& v, const float& min) {

	if (v < min) {

		return true;
	}

	return false;
}

bool Math::isOutOfRangeMax(const float& v, const float& max) {

	if (v > max) {

		return true;
	}

	return false;
}



//=============================================
// 
//			�����_���Ȓl�𐶐�
// 
//=============================================

//	����
int Math::rand(const int& range, const int& offset) {
	
	return std::rand() % range + offset;
}


//	����
float Math::frand(const float& max = (float(1.0f)), const float& min = (float(1.0f))) {
	
	return static_cast<float>(std::rand()) / RAND_MAX * (max - min) + min;
}



//==============================
//			�x�N�g��
//==============================

//	���K��
void Math::normalize(DirectX::XMFLOAT2& v) {

	return 	DirectX::XMStoreFloat2(&v, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&v)));
}



//==============================
//			�p�x
//==============================
const float Math::toRadians(const float& degrees) {

	return DirectX::XMConvertToRadians(degrees);
}