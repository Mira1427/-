#include "ModelBehavior.h"

void ModelBehavior::execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:
	{
		static int count = 0;
		obj->name_ = "Model" + std::to_string(count++);

		obj->state_++;
	}

	case 1:
	{
		auto* animator = obj->getComponent<AnimatorComponent>();
		if (animator)
		{
			animator->update(elapsedTime);
			animator->updateNodeTransform();
		}

		obj->transform_->transform();

		break;
	}

	}
}
