#include "Camera.h"

#include "Library.h"

// ===== カメラクラス ========================================================================================================================

// --- 更新処理 ---
void Camera::update(float elapsedTime)
{
	// --- ビュー行列作成 ---
	view_.makeLookAt(position_, target_);
	invView_ = Matrix::inverse(view_);

	// --- プロジェクション行列作成 ---
	float aspect = window::getWidth() / window::getHeight();
	projection_.makePerspective(fov_, aspect, nearZ_, farZ_);

	// --- ビュープロジェクション行列作成 ---
	viewProjection_ = view_ * projection_;


	// --- 逆行列からカメラの正面、上、右方向のベクトルを取得 ---
	Matrix world = Matrix::inverse(view_);
	frontVec_ = Vector3::normalize({ world._31, world._32, world._33 });
	upVec_ = Vector3::normalize({ world._21, world._22, world._23 });
	rightVec_ = Vector3::normalize({ world._11, world._12, world._13 });


	// --- 移動 ---
	move(elapsedTime);

	// --- 回転 ---
	rotate(elapsedTime);
}


// --- 移動 ---
void Camera::move(float elapsedTime)
{
	if (input::state(input::RMB))
	{
		const float moveSpeed = 10.0f * elapsedTime;
		position_ += rightVec_ * input::getCursorDeltaX() * moveSpeed;
		target_   += rightVec_ * input::getCursorDeltaX() * moveSpeed;
		position_ += -upVec_   * input::getCursorDeltaY() * moveSpeed;
		target_   += -upVec_   * input::getCursorDeltaY() * moveSpeed;
	}
}


// --- ズーム ---
void Camera::zoom(float amount)
{
	position_ += frontVec_ * amount;
	target_ += frontVec_ * amount;
}


// --- 回転 ---
void Camera::rotate(float elapsedTime)
{
	if (GetAsyncKeyState(VK_MENU) && input::state(input::LMB))
	{
		static Vector3 angle{150, 0.0f, 0.0f};

		const float rotateSpeed = 5.0f * elapsedTime;

		angle.x += -input::getCursorDeltaY() * rotateSpeed;
		angle.y += -input::getCursorDeltaX() * rotateSpeed;
		angle.toRadian();

		Matrix T;
		T.makeRotation(angle.x, angle.y, 0.0f);

		Vector3 front = { T._31, T._32, T._33 };
		front.normalize();

		position_.x = target_.x +  front.x * 15;
		position_.y = target_.y +  front.y * 15;
		position_.z = target_.z + -front.z * 15;
	}
}


// --- デバッグ用GUI描画 ---
void Camera::drawDebugGui()
{
	ImGui::Spacing();
	ImGui::Separator();

	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode(u8"ビュー行列"))
	{
		ImGui::DragFloat3(u8"位置", &position_.x, 0.1f, -10000, 10000);
		ImGui::DragFloat3(u8"注視点", &target_.x, 0.1f, -10000, 10000);
		ImGui::TreePop();
	}

	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode(u8"プロジェクション行列"))
	{
		ImGui::DragFloat(u8"視野角", &fov_, 1.0f, 60.0f, 120.0f);
		ImGui::DragFloat(u8"最近距離", &nearZ_, 0.1f, 0.0f, 1000.0f);
		ImGui::DragFloat(u8"最遠距離", &farZ_, 0.1f, 0.0f, 1000.0f);
		ImGui::TreePop();
	}
}
