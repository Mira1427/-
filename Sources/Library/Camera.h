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
	// --- �C���X�^���X�̎擾 ---
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

	// --- �X�V���� ---
	void update(float elapsedTime);

	// --- �ړ� ---
	void move(float elapsedTime);

	// --- �Y�[�� ---
	void zoom(float amount);

	// --- ��] ---
	void rotate(float elapsedTime);

	// --- �f�o�b�O�pGUI�`�� ---
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