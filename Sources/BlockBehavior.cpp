#include "BlockBehavior.h"
#include "Component.h"

void BlockBehavior::execute(GameObject* obj, float elapsedTime)
{
	auto* collider = obj->getComponent<BoxCollider>();
	auto* block = obj->getComponent<BlockComponent>();

	switch (obj->state_)
	{
	case 0:
	{
		static int count = 0;
		obj->name_ = u8"ƒuƒƒbƒN" + std::to_string(count++);

		obj->transform_->position_.y = 2.5f;
		obj->transform_->scaling_ *= 2.5f;

		collider->size_ = { 5.0f, 5.0f, 5.0f };

		block->life_ = 5;

		obj->state_++;
		[[fallthrough]];
	}
	case 1:

		if (block->life_ <= 0)
			obj->destroy();

		obj->transform_->transform();

		break;
	}
}

void BlockBehavior::hit(GameObject* src, GameObject* dst, float elapsedTime)
{
	if (dst->type_ == ObjectType::PLAYER)
	{
		Vector3 vec = dst->transform_->position_ - src->transform_->position_;
		vec.y = 0.0f;
		vec.normalize();

		vec *= 10;

		dst->transform_->position_ += vec * elapsedTime;
	}
}
