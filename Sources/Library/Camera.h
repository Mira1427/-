#pragma once

#include "../../External/ImGui/imgui.h"
#include "Matrix.h"

class Camera {
private:
	Camera() {};

	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;
	Camera(Camera&&) noexcept = delete;
	Camera& operator=(Camera&&) noexcept = delete;

public:
	// --- インスタンスの取得 ---
	static Camera& instance() {
		static Camera camera;
		return camera;
	}

	const Matrix& getViewProjection() const { return viewProjection_; }
	const Matrix& getView() const { return view_; }
	const Matrix getProjection() const { return projection_; }
	const Matrix& getInvView() const { return invView_; }
	const Vector3& getPosition() const { return position_; }
	const Vector3& getTarget() const { return target_; }

	// --- 更新処理 ---
	void update(float elapsedTime);

	// --- 移動 ---
	void move(float elapsedTime);

	// --- ズーム ---
	void zoom(float amount);

	// --- 回転 ---
	void rotate(float elapsedTime);

	// --- デバッグ用GUI描画 ---
	void drawDebugGui();

private:
	Matrix viewProjection_;
	Matrix view_;
	Matrix invView_;
	Matrix projection_;

	Vector3 position_{ 0.0f, 10.0f, -10.0f };
	Vector3 target_;

	float fov_ = 60.0f;
	float nearZ_ = 0.1f;
	float farZ_ = 1000.0f;

	Vector3 frontVec_;
	Vector3 upVec_;
	Vector3 rightVec_;
};