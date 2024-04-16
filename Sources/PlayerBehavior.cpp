#include "PlayerBehavior.h"

#include "Library/Library.h"

#include "Component.h"


// ===== プレイヤー =========================================================================================================
void PlayerBehavior::execute(GameObject* obj, float elapsedTime)
{
	auto* collider = obj->getComponent<BoxCollider>();

	switch (obj->state_)
	{
	case 0:

		obj->transform_->position_.y = 1.5f;

		collider->size_ = { 2.0f, 2.0f, 2.0f };

		obj->state_++;
		[[fallthrough]];

	case 1:
	{
		auto* rigidBody = obj->getComponent<RigidBodyComponent>();

		float left = 0.0f, right = 0.0f, front = 0.0f, back = 0.0f;

		if (input::state(input::LEFT))
			left = -1.0f;

		if (input::state(input::RIGHT))
			right = 1.0f;

		if (input::state(input::DOWN))
			front = -1.0f;

		if (input::state(input::UP))
			back = 1.0f;

		float ax = right + left;
		float az = front + back;

		rigidBody->velocity_.x = ax * 5.0f * elapsedTime;
		rigidBody->velocity_.z = az * 5.0f * elapsedTime;

		obj->transform_->position_ += rigidBody->velocity_;

		turn(obj, rigidBody->velocity_.xz(), 1000.0f, elapsedTime);

		obj->transform_->transform();

		break;
	}

	}
}


// --- 旋回処理 ---
void PlayerBehavior::turn(GameObject* obj, Vector2 vec, float speed, float elapsedTime)
{
	speed *= elapsedTime;

	if (vec.x == 0 && vec.y == 0)
		return;

	// --- 正規化 ---
	vec.normalize();

	float angle = atan2(vec.y, vec.x);
	angle = -DirectX::XMConvertToDegrees(angle);

	obj->transform_->rotation_.y = angle;
}



// ===== つるはし用 =========================================================================================================
void PickAxeBehavior::execute(GameObject* obj, float elapsedTime)
{
	auto* collider = obj->getComponent<BoxCollider>();

	switch (obj->state_)
	{
	case 0:
		collider->size_ = { 1.0f, 1.0f, 1.0f };
		collider->offset_.y = 2.25f;

		obj->state_++;
		[[fallthrough]];

	case 1:
	{
		// --- 回転行列から前方向を取得 ---
		Matrix rotation = {};
		Quaternion rot = {};
		Vector3 rotationVec = obj->parent_->transform_->rotation_;
		rotationVec.y += 90;
		rot.setRotationDegFromVector(rotationVec);
		rotation.makeRotationFromQuaternion(rot);

		Vector3 front = rotation.v_[2].xyz();
		front *= 5;

		obj->transform_->position_ = obj->parent_->transform_->position_ + front;

		break;
	}

		break;
	}
}

void PickAxeBehavior::hit(GameObject* src, GameObject* dst, float elapsedTime)
{
	if (dst->type_ == ObjectType::BLOCK)
	{
		if (input::trigger(input::LMB))
			dst->getComponent<BlockComponent>()->life_--;
	}
}
