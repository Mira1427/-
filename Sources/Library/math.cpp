#include "math.h"
#include <random>


//===============================
//			クランプ
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
//			ランダムな値を生成
// 
//=============================================

//	整数
int Math::rand(const int& range, const int& offset) {
	
	return std::rand() % range + offset;
}


//	実数
float Math::frand(const float& max = (float(1.0f)), const float& min = (float(1.0f))) {
	
	return static_cast<float>(std::rand()) / RAND_MAX * (max - min) + min;
}



//==============================
//			ベクトル
//==============================

//	正規化
void Math::normalize(DirectX::XMFLOAT2& v) {

	return 	DirectX::XMStoreFloat2(&v, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&v)));
}



//==============================
//			角度
//==============================
const float Math::toRadians(const float& degrees) {

	return DirectX::XMConvertToRadians(degrees);
}