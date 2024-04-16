#pragma once

#include "PointLightBehavior.h"
#include "SpriteBehavior.h"
#include "ModelBehavior.h"
#include "CameraBehavior.h"
#include "PlayerBehavior.h"
#include "BlockBehavior.h"
#include "ButtonBehavior.h"

class BehaviorManager
{
public:
	static PointLightBehavior		pointLightBehavior_;
	static SpriteBehavior			spriteBehavior_;
	static ModelBehavior			modelBehavior_;
	static DebugCameraBehavior		debugCameraBehavior_;
	static PlayerBehavior			playerBehavior_;
	static PickAxeBehavior			pickAxeBehavior_;
	static BlockBehavior			blockBehavior_;
	static ButtonManagerBehavior	buttonManager_;
	static TitleStartButtonBehavior titleStartButton_;
};

