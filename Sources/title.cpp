#include "sceneManager.h"

#include "Graphics/Graphics.h"
#include "Graphics/EffectManager.h"

#include "./Library/Library.h"

#include "BehaviorManager.h"
#include "EraserManager.h"
#include "Component.h"

#include "title.h"
#include "game.h"


void Title::initialize()
{
	Scene::initialize();
}


void Title::deinitialize()
{

}


void Title::update(float elapsedTime) {

	switch (state_) {
	case 0:

		// --- カメラオブジェクト ---
	{
		camera_ = GameObjectManager::instance().add(std::make_shared<GameObject>(), Vector3(), &BehaviorManager::debugCameraBehavior_);

		camera_->addComponent<CameraComponent>();

		camera_->name_ = u8"デバッグカメラ ( タイトル )";

		camera_->eraser_ = &EraserManager::titleEraser_;

		camera_->transform_->position_ = { 0.0f, 20.0f, -20.0f };
		camera_->transform_->rotation_ = { 200.0f, -180.0f, 0.0f };
	}

	// --- ボタン用 ---
	{
		auto* buttonManager = GameObjectManager::instance().add(std::make_shared<GameObject>(), Vector3(), &BehaviorManager::buttonManager_);
		{
			buttonManager->addComponent<ButtonComponent>();

			buttonManager->name_ = u8"ボタン管理";
		}

		auto* startButton = GameObjectManager::instance().add(std::make_shared<GameObject>(), Vector3(), &BehaviorManager::titleStartButton_);
		{
			startButton->name_ = u8"スタートボタン";
			startButton->parent_ = buttonManager;
		}
	}

	{
		
	}

		skyMap_ = std::make_unique<SkyMap>(Graphics::instance().getDevice(), L"./Data/Images/SkyMap/envmap_miramar.dds");

		state_++;

	case 1:

		GameObjectManager::instance().update(elapsedTime);

		GameObjectManager::instance().judgeCollision(elapsedTime);

		GameObjectManager::instance().remove();

		EffectManager::instance().update(elapsedTime);

		camera_->getComponent<CameraComponent>()->update();


		break;
	}
}


void Title::render(ID3D11DeviceContext* dc) {

	Graphics::instance().setDepthStencil(false, false);
	rasterize::set(rasterize::NONE);

	skyMap_->draw(dc);

	{
		Graphics::instance().getDebugLineRenderer()->draw(dc);
	}

	GameObjectManager::instance().draw(dc);

	{
		auto* camera = camera_->getComponent<CameraComponent>();

		EffectManager::instance().render(camera->view_, camera->projection_);
	}

	{
		Graphics::instance().getDebugRenderer()->draw(dc);
	}
}


void Title::ImGui() {

}