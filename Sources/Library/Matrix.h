#pragma once

#include "Vector.h"

// ===== 行列クラス ========================================================================================================================
class Matrix
{
public:
	// --- コンストラクタ ---
	Matrix(
		float m11 = 1.0f, float m12 = 0.0f, float m13 = 0.0f, float m14 = 0.0f,
		float m21 = 0.0f, float m22 = 1.0f, float m23 = 0.0f, float m24 = 0.0f,
		float m31 = 0.0f, float m32 = 0.0f, float m33 = 1.0f, float m34 = 0.0f,
		float m41 = 0.0f, float m42 = 0.0f, float m43 = 0.0f, float m44 = 1.0f)
	{
		_11 = m11; _12 = m12; _13 = m13; _14 = m14;
		_21 = m21; _22 = m22; _23 = m23; _24 = m24;
		_31 = m31; _32 = m32; _33 = m33; _34 = m34;
		_41 = m41; _42 = m42; _43 = m43; _44 = m44;
	}

	Matrix(const Matrix& m)
	{
		*this = m;
	}

	Matrix(const DirectX::XMFLOAT4X4& m)
	{
		mat_ = m;
	}


	operator DirectX::XMMATRIX() const
	{
		return DirectX::XMLoadFloat4x4(&mat_);
	}


	// --- 代入演算子 ---
	Matrix& operator=(const Matrix& m)
	{
		mat_ = m.mat_;
		return *this;
	}

	// --- 乗算代入 ---
	const Matrix& operator*=(const Matrix& m)
	{
		multiply(*this, m);
		return *this;
	}

	// --- 乗算 ---
	void multiply(const Matrix& m0, const Matrix& m1)
	{
		DirectX::XMStoreFloat4x4(
			&mat_,
			DirectX::XMMatrixMultiply(m0, m1)
		);
	}


	// --- 平行移動行列の作成 ---
	void makeTranslation(const Vector3& position)
	{
		DirectX::XMStoreFloat4x4(
			&mat_,
			DirectX::XMMatrixTranslationFromVector(position)
		);
	}

	void makeTranslation(float x, float y, float z)
	{
		makeTranslation(Vector3(x, y, z));
	}


	// --- 回転行列の作成 ( Radian ) ---
	void makeRotation(const Vector3& rotation)
	{
		DirectX::XMStoreFloat4x4(
			&mat_,
			DirectX::XMMatrixRotationRollPitchYawFromVector(rotation)
		);
	}

	void makeRotation(float x, float y, float z)
	{
		makeRotation(Vector3(x, y, z));
	}

	
	// --- クォータニオンから回転行列を作成 ---
	void makeRotationFromQuaternion(const Quaternion& q)
	{
		DirectX::XMStoreFloat4x4(
			&mat_,
			DirectX::XMMatrixRotationQuaternion(q)
		);
	}


	// --- 拡大行列の作成 ---
	void makeScaling(const Vector3& scaling)
	{
		DirectX::XMStoreFloat4x4(
			&mat_,
			DirectX::XMMatrixScalingFromVector(scaling)
		);
	}

	void makeScaling(float x, float y, float z)
	{
		makeScaling(Vector3(x, y, z));
	}


	// --- カメラ行列の作成 ---
	void makeLookAt(const Vector3& position, const Vector3& target, const Vector3& up = Vector3::Up_)
	{
		DirectX::XMStoreFloat4x4(
			&mat_,
			DirectX::XMMatrixLookAtLH(position, target, up)
		);
	}


	/// <summary>
	/// --- 平行投影行列の作成 ---
	/// </summary>
	/// <param name="width">: 画面の幅</param>
	/// <param name="height">: 画面の高さ</param>
	/// <param name="nearZ">: 最近距離</param>
	/// <param name="farZ">: 最遠距離</param>
	void makeOrthographic(float width, float height, float nearZ, float farZ)
	{
		DirectX::XMStoreFloat4x4(
			&mat_,
			DirectX::XMMatrixOrthographicLH(
				width,
				height,
				nearZ,
				farZ
			)
		);
	}

	/// <summary>
	/// --- 透視変換行列の作成 ---
	/// </summary>
	/// <param name="fov">: 視野角 ( 度数法 )</param>
	/// <param name="aspect">: 画面の比率</param>
	/// <param name="nearZ">: 最近距離</param>
	/// <param name="farZ">: 最遠距離</param>
	void makePerspective(float fov, float aspect, float nearZ, float farZ)
	{
		DirectX::XMStoreFloat4x4(
			&mat_,
			DirectX::XMMatrixPerspectiveFovLH(
				DirectX::XMConvertToRadians(fov),
				aspect,
				nearZ,
				farZ
			)
		);
	}


	// --- 逆行列化 ---
	void inverse()
	{
		DirectX::XMStoreFloat4x4(&mat_, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&mat_)));
	}

	static Matrix inverse(const Matrix& m)
	{
		Matrix result;
		result = m;
		result.inverse();
		return result;
	}



	static const Matrix Identify_;

	union
	{
		DirectX::XMFLOAT4X4 mat_;
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		Vector4 v_[4];
		float m_[4][4];
	};
};


static inline Matrix operator*(const Matrix& m0, const Matrix& m1)
{
	Matrix result;
	result.multiply(m0, m1);
	return result;
}

