#pragma once

#include <wrl.h>
#include <list>

#include <d3d11.h>

#include "../Library/Matrix.h"


// ===== デバッグプリミティブ描画クラス ===================================================================================================================
class DebugRenderer
{
public:
	// --- コンストラクタ ---
	DebugRenderer(ID3D11Device* device);


	/// <summary>
	/// --- 長方形の描画 ---
	/// </summary>
	/// <param name="position">: 位置</param>
	/// <param name="size_">: サイズ</param>
	/// <param name="rotation_">: 回転量</param>
	/// <param name="color_">: 色</param>
	/// <remarks>
	/// 実際にはリストに追加しているだけ
	/// </remarks>
	void drawRectangle(const Vector2& position, const Vector2& size_, const Vector2& center = Vector2(0.0f, 0.0f), float rotation = 0.0f, const Vector4 & color_ = Vector4::Black_);


	/// <summary>
	/// --- 円の描画 ---
	/// </summary>
	/// <param name="position">: 位置</param>
	/// <param name="radius">: 半径</param>
	/// <param name="rotation">: 回転量</param>
	/// <param name="color">: 色</param>
	/// <remarks>
	/// 実際にはリストに追加しているだけ
	/// </remarks>
	void drawCircle(const Vector2& position, float radius = 1.0f, const Vector4& color = Vector4::Black_);


	/// <summary>
	/// --- 立方体の描画 ---
	/// </summary>
	/// <param name="position">: 位置</param>
	/// <param name="size">: サイズ</param>
	/// <param name="rotation">: 回転量</param>
	/// <param name="color">: 色</param>
	/// <remarks>
	/// 実際にはリストに追加しているだけ
	/// </remarks>
	void drawCube(const Vector3& position, const Vector3& size = Vector3::Unit_, const Vector3& rotation = Vector3::Zero_, const Vector4& color = Vector4::Black_);


	/// <summary>
	/// --- 球の描画 ---
	/// </summary>
	/// <param name="position">: 位置</param>
	/// <param name="radius">: 半径</param>
	/// <param name="color">: 色</param>
	/// <remarks>
	/// 実際にはリストに追加しているだけ
	/// </remarks>
	void drawSphere(const Vector3& position, float radius = 1.0f, const Vector4& color = Vector4::Black_);


	/// <summary>
	/// --- 円柱の描画 ---
	/// </summary>
	/// <param name="position">: 位置</param>
	/// <param name="radius">: 半径</param>
	/// <param name="height">: 高さ</param>
	/// <param name="rotation">: 回転量</param>
	/// <param name="color">: 色</param>
	/// <remarks>
	/// 実際にはリストに追加しているだけ
	/// </remarks>
	void drawCapsule(const Vector3& position, float radius = 1.0f, float height = 1.0f, const Vector3& rotation = Vector3::Zero_, const Vector4& color = Vector4::Black_);


	// --- 描画 ---
	void draw(ID3D11DeviceContext* immediateComtext);

private:
	// --- 長方形の作成 ---
	void createRectangle(ID3D11Device* device);

	// --- 円の作成 ---
	void createCircle(ID3D11Device* device, int vertexNum);

	// --- 立方体の作成 ---
	void createCube(ID3D11Device* device);

	// --- 球の作成 ---
	void createSphere(ID3D11Device* device, int vertexNum);

	// --- 円柱の作成 ---
	void createCapsule(ID3D11Device* device, int vertexNum);


	// --- 長方形のオブジェクトデータ ---
	struct Rectangle
	{
		Vector4 color_;
		Vector2 position_;
		Vector2 size_;
		Vector2 center_;
		float	rotation_;
	};
	std::list<Rectangle> rectangles_;								// オブジェクトのリスト
	Microsoft::WRL::ComPtr<ID3D11Buffer> rectangleVertexBuffer_;	// 頂点バッファ


	// --- 円のオブジェクトデータ ---
	struct Circle
	{
		Vector4 color_;
		Vector2 position_;
		float	radius_;
	};
	std::list<Circle> circles_;									// オブジェクトのリスト
	Microsoft::WRL::ComPtr<ID3D11Buffer> circleVertexBuffer_;	// 頂点バッファ
	size_t circleVertexCount_;									// 頂点の数


	// --- 立方体のオブジェクトデータ
	struct Cube
	{
		Vector4 color_;
		Vector3 position_;
		Vector3 size_;
		Vector3 rotation_;
	};
	std::list<Cube> cubes_;									// オブジェクトのリスト
	Microsoft::WRL::ComPtr<ID3D11Buffer> cubeVertexBuffer_;	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> cubeIndexBuffer_;	// インデックスバッファ


	// --- 球のオブジェクトデータ ---
	struct Sphere
	{
		Vector4 color_;
		Vector3 position_;
		float	radius_;
	};
	std::list<Sphere> spheres_;									// オブジェクトのリスト
	Microsoft::WRL::ComPtr<ID3D11Buffer> sphereVertexBuffer_;	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> sphereIndexBuffer_;	// インデックスバッファ


	// --- 円柱のオブジェクトデータ ---
	struct Capsule
	{
		Vector4 color_;
		Vector3 position_;
		float	radius_;
		Vector3 rotation_;
		float	height_;
	};
	std::list<Capsule> Capsules_;									// オブジェクトのリスト
	Microsoft::WRL::ComPtr<ID3D11Buffer> CapsuleVertexBuffer_;		// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> CapsuleIndexBuffer_;		// インデックスバッファ


	struct Constants3D
	{
		Matrix	world_;
		Vector4 color_;
	};

	struct Constants2D
	{
		Vector2	position_;
		Vector2	size_;
		Vector4	color_;
		Vector2	center_;
		float	rotation_;
	};


	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader_;		// 頂点シェーダー
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader2D_;	
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader_;		// ピクセルシェーダー
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout_;		// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11Buffer>		constantBuffer_;	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>		constantBuffer2D_;
};

