#include "GameObject.h"

#include "./Library/library.h"
#include "./Library/Collision.h"

#include "sceneManager.h"


// ===== コンポーネント =======================================================================================================================================

// --- ワールド座標への変換 ---
void Transform::transform()
{
	Matrix translation;
	translation.makeTranslation(position_);

	Matrix scaling;
	scaling.makeScaling(scaling_);

	Matrix rotation;
	Quaternion orientation;
	orientation.setRotationDegFromVector(rotation_);
	rotation.makeRotationFromQuaternion(orientation);

	world_ = scaling * rotation * translation;
}


// --- 2D画像の描画 ---
void SpriteRendererComponent::draw(ID3D11DeviceContext* dc)
{
	if (!pTexture_)
		return;

	if (!isVisible_)
		return;

	blend::set(blendState_);
	rasterize::set(rasterState_);
	Graphics::instance().setDepthStencil(testDepth_, writeDepth_);

	Graphics::instance().getSpriteRenderer()->draw(
		dc,
		*pTexture_,
		object_->transform_->position_,
		object_->transform_->scaling_,
		texPos_,
		texSize_,
		center_,
		object_->transform_->rotation_,
		color_,
		inWorld_,
		useBillboard_
	);

	
	if (object_->isChoose_ && !useBillboard_ && !inWorld_)
	{
		Vector2 size = { texSize_.x * object_->transform_->scaling_.x, texSize_.y * object_->transform_->scaling_.y };

		Graphics::instance().getDebugRenderer()->drawRectangle(
			{ object_->transform_->position_.x, object_->transform_->position_.y },
			size,
			center_,
			object_->transform_->rotation_.x,
			{ 1.0f, 1.0f, 0.0f, 1.0f }
		);
	}
}


// --- マスク描画 ---
void MaskRendererComponent::draw(ID3D11DeviceContext* dc)
{
	if (!texture_)
		return;

	if (!maskTexture_)
		return;

	if (!isDraw_)
		return;

	blend::set(blendState_);
	rasterize::set(rasterState_);
	Graphics::instance().setDepthStencil(1);

	Graphics::instance().getSpriteRenderer()->draw(
		dc,
		*maskTexture_,
		object_->transform_->position_,
		object_->transform_->scaling_,
		texPos_,
		texSize_,
		center_,
		object_->transform_->rotation_,
		color_,
		inWorld_,
		useBillboard_
	);

	Graphics::instance().setDepthStencil(depthState_);

	Graphics::instance().getSpriteRenderer()->draw(
		dc,
		*texture_,
		object_->transform_->position_ + offset_,
		object_->transform_->scaling_,
		texPos_,
		texSize_,
		center_,
		object_->transform_->rotation_,
		color_,
		inWorld_,
		useBillboard_
	);


	if (object_->isChoose_ && !useBillboard_ && !inWorld_)
	{
		Graphics::instance().setDepthStencil(0);

		Vector2 size = { texSize_.x * object_->transform_->scaling_.x, texSize_.y * object_->transform_->scaling_.y };

		Graphics::instance().getDebugRenderer()->drawRectangle(
			object_->transform_->position_.xy() + offset_.xy(),
			size,
			center_,
			object_->transform_->rotation_.x,
			{ 1.0f, 1.0f, 0.0f, 1.0f }
		);
	}
}


// --- 2D画像のアニメーション更新 ---
void AnimationComponent::update(float elapsedTime)
{
	SpriteRendererComponent* spriteRenderer = object_->getComponent<SpriteRendererComponent>();

	if (!spriteRenderer)
		return;

	if (!animationClips_)
		return;

	if (animationClips_->size() == 0)
		return;

	AnimationData& animeData = animationClips_->at(static_cast<size_t>(animationIndex));

	spriteRenderer->pTexture_ = animeData.pTexture_;

	if (!spriteRenderer->pTexture_)
		return;

	endFrame_ = static_cast<int>(animeData.pFrameData_->size() - 1);


	// --- 再生中 ---
	if (isPlay_)
	{
		frame_ += timeScale_ * elapsedTime;
	}

	// --- 終了フレームに到達したら ---
	if (frame_ > animeData.pFrameData_->at(static_cast<size_t>(frameIndex)).frame_)
	{
		frameIndex++;
		frame_ = 0.0f;

		if (frameIndex > endFrame_)
		{
			if (isLoop_)
			{
				frameIndex = startFrame_;
			}

			else
			{
				isPlay_ = false;
				frameIndex = endFrame_;
			}
		}
	}

	spriteRenderer->texPos_ = animeData.pFrameData_->at(static_cast<size_t>(frameIndex)).texPos_;
	spriteRenderer->texSize_ = animeData.pFrameData_->at(static_cast<size_t>(frameIndex)).texSize_;
}

void AnimationComponent::updateDebugGui(float elapsedTime)
{
	SpriteRendererComponent* spriteRenderer = object_->getComponent<SpriteRendererComponent>();

	if (!spriteRenderer)
	{
		ImGui::Text(u8"コンポーネントがありません");
		return;
	}

	if (!spriteRenderer->pTexture_)
	{
		ImGui::Text(u8"テクスチャがありません");
		return;
	}

	if (!animationClips_)
	{
		ImGui::Text(u8"アニメーションがありません");
		return;
	}

	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Animation")) {
		ImGui::Spacing();

		std::vector<const char*> animeNames;

		for (auto& animation : *animationClips_)
			animeNames.emplace_back(animation.name_.c_str());


		if (ImGui::Combo(u8"アニメーション", &animationIndex, animeNames.data(), static_cast<int>(animeNames.size())))
		{
			frameIndex = 0;
			frame_ = 0.0f;
		}

		if (ImGui::Button(u8"再生", { 35, 20 }))
		{
			isPlay_ = true;
			frame_ = static_cast<float>(startFrame_);
			frameIndex = startFrame_;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"一時停止", { 60, 20 }))
		{
			isPlay_ = !isPlay_;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"停止", { 35, 20 }))
		{
			isPlay_ = false;
		}
		ImGui::SameLine();
		ImGui::Checkbox(u8"ループ", &isLoop_);


		ImGui::DragFloat(u8"タイムスケール", &timeScale_, 0.1f);
		ImGui::InputInt(u8"フレーム", &frameIndex);
		ImGui::InputInt(u8"開始フレーム", &startFrame_);
		ImGui::InputInt(u8"終了フレーム", &endFrame_);

		ImGui::TreePop();
	}

}


// --- SkinnedMeshの描画 ---
void SkinnedMeshRendererComponent::draw(ID3D11DeviceContext* dc)
{
	if (!pModel_)
		return;

	if (!isVisible_)
		return;

	AnimatorComponent* animator = object_->getComponent<AnimatorComponent>();

	blend::set(blendState_);
	rasterize::set(rasterState_);
	Graphics::instance().setDepthStencil(testDepth_, writeDepth_);

	if (animator)
	{
		pModel_->render(
			dc,
			object_->transform_->world_.mat_,
			color_.vec_,
			((animator->keyFrame_) ? animator->keyFrame_ : nullptr)
		);
	}

	else
	{
		pModel_->render(
			dc,
			object_->transform_->world_.mat_,
			color_.vec_,
			nullptr
		);
	}
}


// --- SkinnedMeshのアニメーションの更新 ---
void AnimatorComponent::update(float elapsedTime)
{
	SkinnedMeshRendererComponent* meshRenderer = object_->getComponent<SkinnedMeshRendererComponent>();

	// --- コンポーネントがあるかどうか ---
	if (!meshRenderer)
		return;

	// --- モデルが読み込まれているかどうか ---
	if (!meshRenderer->pModel_)
		return;

	Animation& animation{ meshRenderer->pModel_->animationClips.at(static_cast<size_t>(animationIndex)) };
	endFrame_ = static_cast<int>(animation.sequence.size() - 1);

	// --- 再生中 ---
	if (isPlay_)
	{
		frame_ += timeScale_ * elapsedTime;
		keyFrameIndex = static_cast<int>(frame_);
	}

	// --- 終了フレームに到達したら ---
	if (keyFrameIndex > endFrame_)
	{
		// --- ループ中 ---
		if (isLoop_)
		{
			// --- 開始フレームに戻す ---
			frame_ = static_cast<float>(startFrame_);
			keyFrameIndex = startFrame_;
		}

		// --- それ以外 ---
		else
		{
			// --- 再生を停止して、終了フレームで止める ---
			isPlay_ = false;
			frame_ = static_cast<float>(endFrame_);
			keyFrameIndex = endFrame_;
		}
	}

	keyFrame_ = &animation.sequence.at(static_cast<size_t>(keyFrameIndex));
}


// --- SkinnedMeshのノードの更新 ---
void AnimatorComponent::updateNodeTransform()
{
	SkinnedMeshRendererComponent* meshRenderer = object_->getComponent<SkinnedMeshRendererComponent>();

	// --- コンポーネントがあるかどうか ---
	if (!meshRenderer)
		return;

	// --- モデルが読み込まれているかどうか ---
	if (!meshRenderer->pModel_)
		return;

	auto& node = keyFrame_->nodes.at(nodeIndex);
	node.translation = translation_.vec_;
	node.scaling = scaling_.vec_;
	Quaternion orientation;
	orientation.setRotationDegFromVector(rotation_);
	node.rotation = orientation.vec_;


	meshRenderer->pModel_->updateAnimation(*keyFrame_);
}


// --- アニメーターのデバッグGuiの更新 ---
void AnimatorComponent::updateDebugGui(float elapsedTime)
{
	SkinnedMeshRendererComponent* meshRenderer = object_->getComponent<SkinnedMeshRendererComponent>();

	if (!meshRenderer)
	{
		ImGui::Text(u8"コンポーネントがありません");
		return;
	}

	if (!meshRenderer->pModel_)
	{
		ImGui::Text(u8"モデルが読み込まれていません");
		return;
	}

	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Animator")) {
		ImGui::Spacing();

		std::vector<const char*> animeNames;

		for (auto& animation : meshRenderer->pModel_->animationClips)
			animeNames.emplace_back(animation.name.c_str());


		if (ImGui::Combo(u8"アニメーション", &animationIndex, animeNames.data(), static_cast<int>(animeNames.size())))
		{
			keyFrameIndex = 0;
			frame_ = 0.0f;
		}

		if (ImGui::Button(u8"再生", { 35, 20 }))
		{
			isPlay_ = true;
			frame_ = static_cast<float>(startFrame_);
			keyFrameIndex = startFrame_;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"一時停止", { 60, 20 }))
		{
			isPlay_ = !isPlay_;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"停止", { 35, 20 }))
		{
			isPlay_ = false;
		}
		ImGui::SameLine();
		ImGui::Checkbox(u8"ループ", &isLoop_);


		ImGui::DragFloat(u8"タイムスケール", &timeScale_, 0.1f);
		ImGui::InputInt(u8"フレーム", &keyFrameIndex);
		ImGui::InputInt(u8"開始フレーム", &startFrame_);
		ImGui::InputInt(u8"終了フレーム", &endFrame_);


		// --- ノードのデバッグ ---
		if (keyFrame_)
		{
			ImGui::Spacing();

			std::vector<const char*> nodeNames;

			for (auto& node : meshRenderer->pModel_->sceneView.nodes)
				nodeNames.emplace_back(node.name.c_str());

			if (ImGui::Combo(u8"ノード", &nodeIndex, nodeNames.data(), static_cast<int>(nodeNames.size())))
			{
				auto& node = keyFrame_->nodes.at(nodeIndex);
				translation_.vec_ = node.translation;
				scaling_.vec_ = node.scaling;
				rotation_ = { node.rotation.x, node.rotation.y, node.rotation.z };
			}

			ImGui::DragFloat3(u8"位置", &translation_.x);

			static bool isUniform;
			if (ImGui::DragFloat3(u8"スケール", &scaling_.x, 0.01f))
			{
				if (isUniform)
					scaling_.z = scaling_.y = scaling_.x;
			}
			ImGui::SameLine();
			ImGui::Checkbox("##bool", &isUniform);

			ImGui::DragFloat3(u8"回転", &rotation_.x, DirectX::XMConvertToRadians(5));

		}


		ImGui::TreePop();
	}
}


// --- StaticMeshの描画処理 ---
void StaticMeshRendererComponent::draw(ID3D11DeviceContext* dc)
{
	if (!model_)
		return;

	if (!isVisible_)
		return;

	blend::set(blendState_);
	rasterize::set(rasterState_);
	Graphics::instance().setDepthStencil(testDepth_, writeDepth_);

	model_->render(dc, object_->transform_->world_.mat_, color_.vec_);
}


// --- 2Dの矩形の判定領域の描画 ---
void BoxCollider2D::draw(ID3D11DeviceContext* dc)
{
	if (!isVisible_)
		return;

	Vector2 position = object_->transform_->position_.xy() + offset_;

	Graphics::instance().getDebugRenderer()->drawRectangle(
		position,
		size_,
		{0.0f, 0.0f},
		0.0f,
		Vector4::White_
	);
}


// --- 2Dの円の判定領域の描画 ---
void CircleCollider::draw(ID3D11DeviceContext* dc)
{
	if (!isVisible_)
		return;

	Vector2 position = object_->transform_->position_.xy() + offset_;

	Graphics::instance().getDebugRenderer()->drawCircle(
		position,
		radius_,
		Vector4::White_
	);

}


// --- 立方体の判定領域の描画 ---
void BoxCollider::draw(ID3D11DeviceContext* dc)
{
	if (!isVisible_)
		return;

	Graphics::instance().getDebugRenderer()->drawCube(
		object_->transform_->position_ + offset_,
		size_,
		Vector3::Zero_
	);
}


// --- 円柱の判定領域の描画 ---
void CapsuleCollider::draw(ID3D11DeviceContext* dc)
{
	if (!isVisible_)
		return;

	Graphics::instance().getDebugRenderer()->drawCapsule(
		object_->transform_->position_ + offset_,
		radius_,
		height_
	);
}


// --- 球の判定領域の描画 ---
void SphereCollider::draw(ID3D11DeviceContext* dc)
{
	if (!isVisible_)
		return;

	Graphics::instance().getDebugRenderer()->drawSphere(
		object_->transform_->position_ + offset_,
		radius_
	);
}



// --- ポイントライトの更新処理 ---
void PointLightComponent::update()
{
	if (!isOn_)
		return;

	IlluminationManager::PointLight pointLight;
	pointLight.color_		=	color_;
	pointLight.position_	=	object_->transform_->position_;
	pointLight.range_		=	range_;
	pointLight.intensity_	=	intensity_;

	Graphics::instance().getIlluminationManager()->pointLights_.emplace_back(pointLight);
}


// --- ポイントライトのデバッグ球描画 ---
void PointLightComponent::draw(ID3D11DeviceContext* dc)
{
	if (!isVisible_)
		return;

	Graphics::instance().getDebugRenderer()->drawSphere(
		object_->transform_->position_,
		0.5f,
		Vector4::White_
	);
}


// --- カメラの更新処理 ---
void CameraComponent::update()
{
	Constants::Scene data;
	Graphics& graphics = Graphics::instance();

	data.viewProjection_	= viewProjection_;
	data.invViewProjection_ = invViewProjection_;
	data.cameraPosition_ = object_->transform_->position_;

	graphics.updateConstantBuffer<Constants::Scene>(Constants::SCENE, data);

	graphics.getDeviceContext()->VSSetConstantBuffers(
		Constants::SCENE, 1, graphics.constantBuffers[Constants::SCENE].GetAddressOf()
	);

	graphics.getDeviceContext()->PSSetConstantBuffers(
		Constants::SCENE, 1, graphics.constantBuffers[Constants::SCENE].GetAddressOf()
	);
}



// --- カメラのデバッグ描画 ---
void CameraComponent::draw(ID3D11DeviceContext* dc)
{
	if (!isVisible_)
		return;

	Graphics::instance().getDebugRenderer()->drawCube(
		object_->transform_->position_
	);
}


// --- カメラのデバッグGui更新 ---
void CameraComponent::updateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode("Camera")) {
		ImGui::SameLine();
		ImGui::Text("          ");
		ImGui::SameLine();
		ImGui::Checkbox(u8"可視化", &isVisible_);
		ImGui::Spacing();

		if (ImGui::Button(u8"適用", ImVec2(60, 30)))
			SceneManager::instance().scene->camera_ = this->object_;

		ImGui::DragFloat3(u8"目標", &target_.x, 0.1f);
		ImGui::DragFloat(u8"距離", &range_, 0.1f);
		ImGui::DragFloat(u8"視野角", &fov_, 0.1f);
		ImGui::DragFloat(u8"最近距離", &nearZ_, 0.1f);
		ImGui::DragFloat(u8"最遠距離", &farZ_, 0.1f);

		ImGui::TreePop();
	}
}


// ===== ゲームオブジェクトクラス =============================================================================================================================

// --- コンストラクタ ---
GameObject::GameObject() :
	state_(),
	timer_(),
	isChoose_(),
	type_(ObjectType::NONE),
	transform_(std::make_shared<Transform>())
{
	transform_->setObject(this);
	components_.emplace_back(transform_);

	static int objectCount = 0;
	name_ = "Object" + std::to_string(objectCount++);
}


// --- 更新処理 ---
void GameObject::update(float elapsedTime)
{
	if (behavior_)
		behavior_->execute(this, elapsedTime);

	if (eraser_)
		eraser_->execute(this);
}


void GameObject::destroy()
{
	GameObjectManager::instance().discardList_.emplace(this);
}


// ===== ゲームオブジェクト管理クラス =============================================================================================================================

// --- 更新処理 ---
void GameObjectManager::update(float elapsedTime)
{
	for (auto& obj : objectList_)
	{
		obj->update(elapsedTime);
	}
}


// --- 描画処理 ---
void GameObjectManager::draw(ID3D11DeviceContext* dc)
{
	// --- リストのソート ---
	objectList_.sort(
		[](const std::shared_ptr<GameObject>& obj1, const std::shared_ptr<GameObject>& obj2)
		{ return obj1->transform_->position_.z > obj2->transform_->position_.z; }
	);

	for (const auto& obj : objectList_)
	{
		for (const auto& comp : obj->components_)
		{
			comp->draw(dc);
		}
	}
}


// --- 当たり判定の処理 ---
void GameObjectManager::judgeCollision(float elapsedTime)
{
	for (auto& obj : objectList_)
	{
		for (auto& obj2 : objectList_)
		{
			if (obj == obj2)
				continue;

			for (auto& collider : obj->colliders_)
				collider->isHit_ = false;

			for (auto& collider : obj2->colliders_)
				collider->isHit_ = false;

			for (auto& collider : obj->colliders_)
			{
				for (auto& collider2 : obj2->colliders_)
				{
					if (collider->collisionType_ != collider2->collisionType_)
						continue;

					switch (collider->collisionType_)
					{

					case ColliderComponent::CollisionType::BOX2D:
					{
						BoxCollider2D* c1 = obj->getComponent<BoxCollider2D>();
						BoxCollider2D* c2 = obj2->getComponent<BoxCollider2D>();

						if (Collision::intersectRectangles(
							obj->transform_->position_.xy() + c1->offset_, c1->size_,
							obj2->transform_->position_.xy() + c2->offset_, c2->size_))
						{
							c1->isHit_ = true;
							c2->isHit_ = true;

							obj->behavior_->hit(obj.get(), obj2.get(), elapsedTime);
						}
						break;
					}

					case ColliderComponent::CollisionType::CIRCLE:
					{
						CircleCollider* c1 = obj->getComponent<CircleCollider>();
						CircleCollider* c2 = obj2->getComponent<CircleCollider>();

						if (Collision::intersectCircles(
							obj->transform_->position_.xy() + c1->offset_, c1->radius_,
							obj2->transform_->position_.xy() + c2->offset_, c2->radius_))
						{
							c1->isHit_ = true;
							c2->isHit_ = true;

							obj->behavior_->hit(obj.get(), obj2.get(), elapsedTime);
						}
						break;
					}

					case ColliderComponent::CollisionType::BOX:
					{
						BoxCollider* c1 = obj->getComponent<BoxCollider>();
						BoxCollider* c2 = obj2->getComponent<BoxCollider>();

						if (Collision::intersectBoxes(
							obj->transform_->position_ + c1->offset_, c1->size_,
							obj2->transform_->position_ + c2->offset_, c2->size_))
						{
							c1->isHit_ = true;
							c2->isHit_ = true;

							obj->behavior_->hit(obj.get(), obj2.get(), elapsedTime);
						}
						break;
					}


					case ColliderComponent::CollisionType::CAPSULE:
					{
						CapsuleCollider* c1 = obj->getComponent<CapsuleCollider>();
						CapsuleCollider* c2 = obj2->getComponent<CapsuleCollider>();

						if (Collision::intersectCapsules(
							obj->transform_->position_ + c1->offset_, c1->radius_, c1->height_,
							obj2->transform_->position_ + c2->offset_, c2->radius_, c2->height_))
						{
							c1->isHit_ = true;
							c2->isHit_ = true;

							obj->behavior_->hit(obj.get(), obj2.get(), elapsedTime);
						}
						break;
					}


					case ColliderComponent::CollisionType::SPHERE:
					{
						SphereCollider* c1 = obj->getComponent<SphereCollider>();
						SphereCollider* c2 = obj2->getComponent<SphereCollider>();

						if (Collision::intersectSpheres(
							obj->transform_->position_ + c1->offset_, c1->radius_,
							obj2->transform_->position_ + c2->offset_, c2->radius_))
						{
							c1->isHit_ = true;
							c2->isHit_ = true;

							obj->behavior_->hit(obj.get(), obj2.get(), elapsedTime);
						}
						break;
					}

					}
				}
			}
		}
	}

}


// --- オブジェクトの削除 ---
void GameObjectManager::remove()
{
	for (GameObject* object : discardList_)
	{
		auto it = std::find_if(
			objectList_.begin(),
			objectList_.end(),
			[object](const std::shared_ptr<GameObject>& ptr) {return ptr.get() == object; }
		);

		if (it != objectList_.end())
			objectList_.erase(it);
	}

	discardList_.clear();
}


// --- デバッグGuiの更新 ---
void GameObjectManager::updateDebugGui(float elapsedTime)
{
	// --- 位置とサイズを固定 ---
	ImGui::SetNextWindowPos(ImVec2(1550, 10), ImGuiSetCond_Always);
	ImGui::SetNextWindowSize(ImVec2(350, 1020), ImGuiSetCond_Always);

	ImGui::Begin("Inspector");

	for (auto& obj : objectList_)
	{
		if (obj->isChoose_)
		{
			ImGui::Text(u8"選択中のオブジェクト :");
			ImGui::SameLine();
			ImGui::Text(obj->name_.c_str());

			for (auto& component : obj->components_)
			{
				ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
				component->updateDebugGui(elapsedTime);
			}

			if (input::trigger(input::DELETE_))
			{
				obj->destroy();
			}
		}
	}

	ImGui::End();
}


// --- オブジェクトリストの表示 ---
void GameObjectManager::showDebugList()
{
	// --- 位置とサイズを固定 ---
	ImGui::SetNextWindowPos(ImVec2(1340, 10), ImGuiSetCond_Always);
	ImGui::SetNextWindowSize(ImVec2(200, 1020), ImGuiSetCond_Always);

	ImGui::Begin("Hierarchy");

	for (auto& obj : objectList_)
	{
		if (ImGui::Selectable(obj->name_.c_str(), &obj->isChoose_))
		{
			for (auto& it : objectList_)
				it->isChoose_ = false;

			obj->isChoose_ = true;
		}
	}

	ImGui::End();

}


// --- オブジェクトの追加 ---
GameObject* GameObjectManager::add(std::shared_ptr<GameObject> object, const Vector3& position, Behavior* behavior)
{
	object->transform_->position_ = position;
	object->behavior_ = behavior;

	objectList_.emplace_back(object);

	return object.get();
}