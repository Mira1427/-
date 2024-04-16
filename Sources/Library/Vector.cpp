#include "Vector.h"
#include "Matrix.h"

const Vector2 Vector2::Zero_ = { 0.0f, 0.0f };


const Vector3 Vector3::Zero_ = { 0.0f, 0.0f, 0.0f };
const Vector3 Vector3::Unit_ = { 1.0f, 1.0f, 1.0f };

const Vector3 Vector3::Front_ =	{ 0.0f, 0.0f, 1.0f };
const Vector3 Vector3::Back_  =	{ 0.0f, 0.0f, -1.0f };
const Vector3 Vector3::Left_  =	{ -1.0f, 0.0f, 0.0f };
const Vector3 Vector3::Right_ =	{ 1.0f, 0.0f, 0.0f };
const Vector3 Vector3::Up_    =	{ 0.0f, 1.0f, 0.0f };
const Vector3 Vector3::Down_  =	{ 0.0f, -1.0f, 0.0f };

const Vector3 Vector3::AxisX_ = { 1.0f, 0.0f, 0.0f };
const Vector3 Vector3::AxisY_ = { 0.0f, 1.0f, 0.0f };
const Vector3 Vector3::AxisZ_ = { 0.0f, 0.0f, 1.0f };


const Vector4 Vector4::White_ =	{ 1.0f, 1.0f, 1.0f, 1.0 };
const Vector4 Vector4::Black_ =	{ 0.0f, 0.0f, 0.0f, 1.0f };
const Vector4 Vector4::Gray_  =	{ 0.5f, 0.5f, 0.5f, 1.0f };


const Quaternion Quaternion::Identity_ = { 0.0f, 0.0f, 0.0f, 1.0f };


void Quaternion::setRotation(const Matrix& m)
{
	DirectX::XMStoreFloat4(&vec_, DirectX::XMQuaternionRotationMatrix(m));
}