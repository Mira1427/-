#include "sceneManager.h"

#include "Graphics/Graphics.h"
#include "Graphics/EffectManager.h"

#include "./Library/Library.h"

#include "GameObject.h"
#include "BehaviorManager.h"
#include "EraserManager.h"
#include "Component.h"

#include "game.h"
#include "title.h"

void Game::initialize()
{
	Scene::initialize();
}


void Game::deinitialize()
{

}


void Game::update(float elapsedTime)
{

	switch (state_)
	{
	case 0:

		//cube_ = std::make_shared<StaticMesh>(Graphics::instance().getDevice(), L"./Data/Models/ObjTest/test.obj", false);
		cube_ = std::make_shared<StaticMesh>(Graphics::instance().getDevice(), L"./Data/Models/ObjTest/test.obj", false);

		// --- カメラオブジェクト ---
		{
			camera_ = GameObjectManager::instance().add(std::make_shared<GameObject>(), Vector3(), &BehaviorManager::debugCameraBehavior_);

			camera_->addComponent<CameraComponent>();

			camera_->name_ = u8"デバッグカメラ ( ゲーム )";
			camera_->eraser_ = &EraserManager::gameEraser_;

			camera_->transform_->position_ = { 100.0f, 30.0f, -20.0f };
			camera_->transform_->rotation_ = { 200.0f, -200.0f, 0.0f };
		}

		{
			auto* player = GameObjectManager::instance().add(std::make_shared<GameObject>(), Vector3(), &BehaviorManager::playerBehavior_);
			{
				player->name_ = u8"プレイヤー";
				player->type_ = ObjectType::PLAYER;
				player->eraser_ = &EraserManager::gameEraser_;

				player->addComponent<RigidBodyComponent>();
				player->addCollider<BoxCollider>();

#if 0
				auto* MeshRenderer = player->addComponent<SkinnedMeshRendererComponent>();
				MeshRenderer->pModel_ = std::make_unique<SkinnedMesh>(Graphics::instance().getDevice(), "./Data/Models/chara.fbx", true);

				//player->addComponent<AnimatorComponent>();
#else 
				auto* MeshRenderer = player->addComponent<StaticMeshRendererComponent>();
				MeshRenderer->model_ = std::make_unique<StaticMesh>(Graphics::instance().getDevice(), L"./Data/Models/ObjTest/test2.obj", false);
#endif
			}

			auto* pickAxe = GameObjectManager::instance().add(std::make_shared<GameObject>(), Vector3(), &BehaviorManager::pickAxeBehavior_);
			{
				pickAxe->parent_ = player;
				player->child_.emplace_back(pickAxe);
				pickAxe->type_ = ObjectType::PICKAXE;

				pickAxe->name_ = u8"つるはし";
				pickAxe->eraser_ = &EraserManager::gameEraser_;

				pickAxe->addCollider<BoxCollider>();
			}
		}

		for (int x = 0; x < 5; x++)
		{
			for (int z = 0; z < 10; z++)
			{
				addBlock(Vector3(10.0f + 5.0f * x, 0.0f, 5.0f * z));
			}
		}

		skyMap_ = std::make_unique<SkyMap>(Graphics::instance().getDevice(), L"./Data/Images/SkyMap/envmap_miramar.dds");

		state_++;
		[[fallthrough]];

	case 1:

		GameObjectManager::instance().update(elapsedTime);

		GameObjectManager::instance().judgeCollision(elapsedTime);

		GameObjectManager::instance().remove();

		EffectManager::instance().update(elapsedTime);

		camera_->getComponent<CameraComponent>()->update();

		break;
	}
}


void Game::render(ID3D11DeviceContext* dc)
{
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


void Game::ImGui()
{

}

void Game::addBlock(const Vector3& position)
{
	auto* obj = GameObjectManager::instance().add(std::make_shared<GameObject>(), position, &BehaviorManager::blockBehavior_);
	{
		obj->type_ = ObjectType::BLOCK;
		obj->eraser_ = &EraserManager::gameEraser_;

		obj->addCollider<BoxCollider>();
		obj->addComponent<BlockComponent>();

		auto* staticMeshRenderer = obj->addComponent<StaticMeshRendererComponent>();
		staticMeshRenderer->model_ = cube_;
	}
}
