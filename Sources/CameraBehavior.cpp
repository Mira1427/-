#include "CameraBehavior.h"

#include "./Library/Library.h"


// --- カメラの共通処理 ---
void BaseCameraBehavior::execute(GameObject* obj, float elapsedTime)
{
	auto* camera = obj->getComponent<CameraComponent>();

	// --- ビュー行列作成 ---
	camera->view_.makeLookAt(obj->transform_->position_, camera->target_);
	camera->invView_ = Matrix::inverse(camera->view_);

	// --- プロジェクション行列作成 ---
	float aspect = window::getWidth() / window::getHeight();
	camera->projection_.makePerspective(camera->fov_, aspect, camera->nearZ_, camera->farZ_);

	// --- ビュープロジェクション行列作成 ---
	camera->viewProjection_ = camera->view_ * camera->projection_;
	camera->invViewProjection_ = Matrix::inverse(camera->viewProjection_);
	camera->frontVec_ = Vector3::normalize(camera->invView_.v_[2].xyz());
	camera->upVec_ = Vector3::normalize(camera->invView_.v_[1].xyz());
	camera->rightVec_ = Vector3::normalize(camera->invView_.v_[0].xyz());
}


void DebugCameraBehavior::execute(GameObject* obj, float elapsedTime)
{
	BaseCameraBehavior::execute(obj, elapsedTime);

	switch (obj->state_)
	{
	case 0: applyMayaControl(obj, elapsedTime); break;
	}
}


void DebugCameraBehavior::applyMayaControl(GameObject* obj, float elapsedTime)
{
	auto* camera = obj->getComponent<CameraComponent>();

	{
		obj->transform_->position_ += camera->frontVec_ * input::getMouseWheel();
		camera->target_ += camera->frontVec_ * input::getMouseWheel();
	}

	if (input::state(input::MMB))
	{
		const float moveSpeed = 10.0f * elapsedTime;
		const float moveX = static_cast<float>(input::getCursorDeltaX() * moveSpeed);
		const float moveY = static_cast<float>(input::getCursorDeltaY() * moveSpeed);

		obj->transform_->position_ -=  camera->rightVec_ * moveX;
		obj->transform_->position_ += -camera->upVec_	 * moveY;
		camera->target_ -=  camera->rightVec_ * moveX;
		camera->target_ += -camera->upVec_	  * moveY;
	}

	if (GetAsyncKeyState(VK_MENU) && input::state(input::LMB))
	{
		const float rotateSpeed = 20.0f * elapsedTime;
		const float rotateX = -input::getCursorDeltaY() * rotateSpeed;
		const float rotateY = -input::getCursorDeltaX() * rotateSpeed;

		obj->transform_->rotation_.x -= rotateX;
		obj->transform_->rotation_.y += rotateY;
	}

	Vector3 rotation = obj->transform_->rotation_.toRadian();

	Matrix T;
	T.makeRotation(rotation.x, rotation.y, 0.0f);

	Vector3 front = { T._31, T._32, T._33 };
	front.normalize();

	obj->transform_->position_.x = camera->target_.x + front.x  * camera->range_;
	obj->transform_->position_.y = camera->target_.y + front.y  * camera->range_;
	obj->transform_->position_.z = camera->target_.z + -front.z * camera->range_;
}
