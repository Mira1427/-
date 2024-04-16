#include "SpriteBehavior.h"

void SpriteBehavior::execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:
	{
		static int count = 0;
		obj->name_ = "Sprite" + std::to_string(count++);

		obj->state_++;
	}

	case 1:
	{
		auto* animation = obj->getComponent<AnimationComponent>();
		if (animation)
			animation->update(elapsedTime);

		break;
	}

	}
}
