#include "PointLightBehavior.h"

void PointLightBehavior::execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:
	{
		static int count = 0;
		obj->name_ = "PointLight" + std::to_string(count++);

		obj->state_++;
	}

	case 1:
	{
		auto* pointLight = obj->getComponent<PointLightComponent>();
		if (pointLight)
			pointLight->update();

		break;
	}

	}
}
