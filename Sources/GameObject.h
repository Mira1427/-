#pragma once

#include <string>
#include <memory>
#include <vector>
#include <set>

#include <d3d11.h>
#include "../External/ImGui/imgui.h"

#include "Graphics/Graphics.h"
#include "Graphics/SkinnedMesh.h"
#include "Graphics/staticMesh.h"

#include "./Library/String.h"



// ===== 前方宣言 ============================================================================================================================================
class GameObject;


// ===== コンポーネント =======================================================================================================================================
class Component
{
public:
	Component() :
		object_(nullptr)
	{}

	virtual void draw(ID3D11DeviceContext* dc) {}
	virtual void updateDebugGui(float elapsedTime) {};
	void setObject(GameObject* object) { object_ = object; }

protected:
	GameObject* object_;
};


class Transform final : public Component
{
public:
	Transform() :
		world_(),
		position_(),
		scaling_(Vector3::Unit_),
		rotation_()
	{}

	void transform();

	void updateDebugGui(float elapsedTime) override
	{
		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::TreeNode(u8"Transform")) {
			ImGui::Spacing();

			ImGui::DragFloat4(u8"位置", &position_.x, moveSpeed_);

			static bool isUniform;
			if (ImGui::DragFloat4(u8"スケール", &scaling_.x, scaleSpeed_))
			{
				if (isUniform)
					scaling_.z = scaling_.y = scaling_.x;
			}
			ImGui::SameLine();
			ImGui::Checkbox("##bool", &isUniform);

			ImGui::DragFloat4(u8"回転", &rotation_.x, DirectX::XMConvertToRadians(rotateSpeed_));

			ImGui::TreePop();
		}
	}
	
	Matrix world_;
	Vector3 position_;
	float moveSpeed_ = 0.01f;
	Vector3 scaling_;
	float scaleSpeed_ = 0.01f;
	Vector3 rotation_;
	float rotateSpeed_ = 1.0f;
};


class SpriteRendererComponent final : public Component
{
public:
	SpriteRendererComponent() :
		pTexture_(nullptr),
		texPos_(),
		texSize_(),
		center_(),
		color_(Vector4::White_),
		blendState_(1/*Alpha*/),
		rasterState_(0/*CullNone*/),
		testDepth_(true),
		writeDepth_(true),
		inWorld_(),
		isVisible_(true),
		useBillboard_()
	{}

	void draw(ID3D11DeviceContext* dc) override;

	void updateDebugGui(float elapsedTime) override
	{
		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::TreeNode("SpriteRenderer")) {
			ImGui::SameLine();
			ImGui::Text("          ");
			ImGui::SameLine();
			ImGui::Checkbox(u8"可視化", &isVisible_);
			ImGui::Spacing();

			ImGui::BulletText(u8"テクスチャ名 :");
			ImGui::SameLine();
			ImGui::Text(((pTexture_) ? pTexture_->fileName_ : u8"テクスチャがありません"));

			static const char* blendStates[] = { u8"なし", u8"透明", u8"加算" };
			ImGui::Combo(u8"ブレンド", &blendState_, blendStates, ARRAYSIZE(blendStates));

			static const char* rasterStates[] = { u8"カリングなし", u8"ソリッド", u8"ワイヤーフレーム", u8"ワイヤーフレーム カリングなし" };
			ImGui::Combo("ラスター", &rasterState_, rasterStates, ARRAYSIZE(rasterStates));

			ImGui::Checkbox(u8"深度テスト", &testDepth_);
			ImGui::Checkbox(u8"深度書き込み", &writeDepth_);

			ImGui::DragFloat2(u8"切り抜き位置", &texPos_.x);
			ImGui::DragFloat2(u8"切り抜きサイズ", &texSize_.x);
			ImGui::DragFloat2(u8"中心", &center_.x);
			ImGui::ColorEdit4(u8"色", &color_.x);
			ImGui::Checkbox(u8"ワールド空間", &inWorld_);
			ImGui::Checkbox(u8"ビルボード", &useBillboard_);

			ImGui::TreePop();
		}
	}

	Texture* pTexture_;
	Vector2 texPos_;
	Vector2 texSize_;
	Vector2 center_;
	Vector4 color_;
	int		blendState_;
	int		rasterState_;
	bool	testDepth_;
	bool	writeDepth_;
	bool	inWorld_;
	bool	isVisible_;
	bool	useBillboard_;
};


class MaskRendererComponent final : public Component
{
public:
	MaskRendererComponent() :
		texture_(nullptr),
		maskTexture_(nullptr),
		texPos_(),
		texSize_(),
		center_(),
		color_(Vector4::White_),
		offset_(),
		blendState_(1/*Alpha*/),
		rasterState_(0/*CullNone*/),
		depthState_(0/*None*/),
		depthState2_(0/*None*/),
		inWorld_(),
		isDraw_(true),
		useBillboard_()
	{}

	void draw(ID3D11DeviceContext* dc) override;

	void updateDebugGui(float elapsedTime) override
	{
		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::TreeNode("MaskRenderer")) {
			ImGui::SameLine();
			ImGui::Text("          ");
			ImGui::SameLine();
			ImGui::Checkbox("isDraw", &isDraw_);
			ImGui::Spacing();

			ImGui::BulletText("TextureName :");
			ImGui::SameLine();
			ImGui::Text(((texture_) ? texture_->fileName_ : u8"テクスチャがありません"));

			ImGui::BulletText("MaskTextureName :");
			ImGui::SameLine();
			ImGui::Text(((maskTexture_) ? maskTexture_->fileName_ : u8"テクスチャがありません"));

			static const char* blendStates[] = { "None", "Alpha", "Add" };
			ImGui::Combo("BlendState", &blendState_, blendStates, ARRAYSIZE(blendStates));

			static const char* rasterStates[] = { "CullNone", "Solid", "WireFrame", "WireFrame_CullNone" };
			ImGui::Combo("RasterState", &rasterState_, rasterStates, ARRAYSIZE(rasterStates));

			static const char* depthStates[] = { "None", "Mask", "ApplyMask", "Exclusive" };
			ImGui::Combo("DepthState", &depthState_, depthStates, ARRAYSIZE(depthStates));
			ImGui::Combo("DepthState2", &depthState2_, depthStates, ARRAYSIZE(depthStates));

			ImGui::DragFloat3("Offset", &offset_.x);
			ImGui::DragFloat2("TexPos", &texPos_.x);
			ImGui::DragFloat2("TexSize", &texSize_.x);
			ImGui::DragFloat2("Center", &center_.x);
			ImGui::ColorEdit4("Color", &color_.x);
			ImGui::Checkbox("inWorld", &inWorld_);
			ImGui::Checkbox("useBillboard", &useBillboard_);

			ImGui::TreePop();
		}
	}

	Texture* texture_;
	Texture* maskTexture_;
	Vector2 texPos_;
	Vector2 texSize_;
	Vector2 center_;
	Vector4 color_;
	Vector3 offset_;
	int		blendState_;
	int		rasterState_;
	int		depthState_;
	int		depthState2_;
	bool	inWorld_;
	bool	isDraw_;
	bool	useBillboard_;
};


struct AnimationFrameData
{
	float frame_;
	Vector2 texPos_;
	Vector2 texSize_;
};

struct AnimationData
{
	AnimationData() {}

	AnimationData(const std::string& name, Texture* pTexture, std::vector<AnimationFrameData>* pFrameData) :
		name_(name),
		pTexture_(pTexture),
		pFrameData_(pFrameData)
	{}

	std::string name_;
	Texture* pTexture_;
	std::vector<AnimationFrameData>* pFrameData_;
};

class AnimationComponent final : public Component
{
public:
	AnimationComponent() :
		animationClips_(nullptr),
		frame_(),
		animationIndex(),
		frameIndex(),
		timeScale_(1.0f),
		startFrame_(),
		endFrame_(),
		isLoop_(),
		isPlay_()
	{}

	void update(float elapsedTime);

	void updateDebugGui(float elapsedTime) override;

	std::vector<AnimationData>* animationClips_;
	float frame_;
	int animationIndex;
	int frameIndex;
	float timeScale_;
	int startFrame_;
	int endFrame_;
	bool isLoop_;
	bool isPlay_;
};


class SkinnedMeshRendererComponent final : public Component
{
public:
	SkinnedMeshRendererComponent() :
		pModel_(nullptr),
		color_(Vector4::White_),
		meshIndex(),
		boneIndex(),
		blendState_(1/*Alpha*/),
		rasterState_(1/*Solid*/),
		testDepth_(true),
		writeDepth_(true),
		isVisible_(true)
	{}

	void draw(ID3D11DeviceContext* dc) override;

	void updateDebugGui(float elapsedTime) override
	{
		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::TreeNode("SkinnedMeshRenderer")) {
			ImGui::SameLine();
			ImGui::Text("          ");
			ImGui::SameLine();
			ImGui::Checkbox(u8"可視化", &isVisible_);
			ImGui::Spacing();

			if (pModel_)
			{
				std::vector<const char*> boneNames;
				std::vector<const char*> meshNames;

				static const char* blendStates[] = { u8"なし", u8"透明", u8"加算" };
				ImGui::Combo(u8"ブレンド", &blendState_, blendStates, ARRAYSIZE(blendStates));

				static const char* rasterStates[] = { u8"カリングなし", u8"ソリッド", u8"ワイヤーフレーム", u8"ワイヤーフレーム カリングなし" };
				ImGui::Combo(u8"ラスター", &rasterState_, rasterStates, ARRAYSIZE(rasterStates));

				ImGui::Checkbox(u8"深度テスト", &testDepth_);
				ImGui::Checkbox(u8"深度書き込み", &writeDepth_);



				for (auto& mesh : pModel_->meshes)
					meshNames.emplace_back(mesh.name.c_str());

				if (ImGui::Combo(u8"メッシュ", &meshIndex, meshNames.data(), static_cast<int>(meshNames.size())))
					boneIndex = 0;

				for (auto& bone : pModel_->meshes.at(meshIndex).bindPose.bones)
					boneNames.emplace_back(bone.name.c_str());

				ImGui::Combo(u8"ボーン", &boneIndex, boneNames.data(), static_cast<int>(boneNames.size()));

				if (ImGui::TreeNode(u8"マテリアル"))
				{
					ImGui::Text(u8"マテリアルの数 : %d", static_cast<int>(pModel_->materials.size()));

					// --- 一旦先頭のマテリアルだけ取得 ---
					auto& material = pModel_->materials.begin();

					for (auto& srv : material->second.shaderResourceViews)
					{
						ImGui::Image(srv.Get(), ImVec2(100.0f, 100.0f));
					}


					ImGui::TreePop();
				}

				ImGui::ColorEdit4(u8"色", &color_.r);
			}

			ImGui::TreePop();
		}
	}

	std::unique_ptr<SkinnedMesh> pModel_;
	Vector4 color_;
	int meshIndex;
	int boneIndex;
	int blendState_;
	int rasterState_;
	bool testDepth_;
	bool writeDepth_;
	bool isVisible_;
};


class AnimatorComponent final : public Component
{
public:
	AnimatorComponent() :
		keyFrame_(nullptr),
		animationIndex(),
		keyFrameIndex(),
		startFrame_(),
		endFrame_(),
		frame_(),
		timeScale_(1.0f),
		isLoop_(),
		isPlay_(),
		nodeIndex(),
		translation_(),
		scaling_(1.0f, 1.0f, 1.0f),
		rotation_()
	{}

	void update(float elapsedTime);
	void updateNodeTransform();

	void updateDebugGui(float elapsedTime) override;


	Animation::KeyFrame* keyFrame_;
	int		animationIndex;
	int		keyFrameIndex;
	int		startFrame_;
	int		endFrame_;
	float	frame_;
	float	timeScale_;
	bool	isLoop_;
	bool	isPlay_;

	int		nodeIndex;
	Vector3 translation_;
	Vector3 scaling_;
	Vector3 rotation_;
};


class StaticMeshRendererComponent final : public Component
{
public:
	StaticMeshRendererComponent() :
		model_(nullptr),
		color_(1.0f, 1.0f, 1.0f, 1.0f),
		blendState_(1/*Alpha*/),
		rasterState_(1/*Solid*/),
		materialIndex_(),
		testDepth_(true),
		writeDepth_(true),
		isVisible_(true)
	{}

	void draw(ID3D11DeviceContext* dc) override;

	void updateDebugGui(float elapsedTime) override
	{
		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::TreeNode("StaticMeshRenderer")) {
			ImGui::SameLine();
			ImGui::Text("          ");
			ImGui::SameLine();
			ImGui::Checkbox(u8"可視化", &isVisible_);
			ImGui::Spacing();

			if (model_)
			{
				static const char* blendStates[] = { u8"なし", u8"透明", u8"加算" };
				ImGui::Combo(u8"ブレンド", &blendState_, blendStates, ARRAYSIZE(blendStates));

				static const char* rasterStates[] = { u8"カリングなし", u8"ソリッド", u8"ワイヤーフレーム", u8"ワイヤーフレーム カリングなし" };
				ImGui::Combo(u8"ラスター", &rasterState_, rasterStates, ARRAYSIZE(rasterStates));

				ImGui::Checkbox(u8"深度テスト", &testDepth_);
				ImGui::Checkbox(u8"深度書き込み", &writeDepth_);

				if (ImGui::TreeNode(u8"マテリアル"))
				{
					std::vector<const char*> materialNames;
					for (auto& material : model_->materials)
						materialNames.emplace_back(WideCharToUTF8(material.name.c_str()).c_str());

					ImGui::Combo(u8"マテリアル", &materialIndex_, materialNames.data(), static_cast<int>(materialNames.size()));
					auto& material = model_->materials.at(materialIndex_);

					ImGui::Text(u8"%s", WideCharToUTF8(material.name.c_str()).c_str());

					for (int i = 0; i < 2; i++)
					{
						ImGui::Text(u8"%s", WideCharToUTF8(material.textureFileNames[i].c_str()).c_str());
						ImGui::Image(material.shaderResourceViews[i].Get(), ImVec2(100.0f, 100.0f));
					}


					ImGui::TreePop();
				}

				ImGui::ColorEdit4(u8"色", &color_.r);
			}

			else
				ImGui::Text(u8"モデルが読み込まれていません");

			ImGui::TreePop();
		}

	}

	std::shared_ptr<StaticMesh> model_;
	Vector4 color_;
	int blendState_;
	int rasterState_;
	int materialIndex_;
	bool testDepth_;
	bool writeDepth_;
	bool isVisible_;
};


class ColliderComponent : public Component
{
public:
	enum class CollisionType
	{
		BOX2D,
		CIRCLE,
		BOX,
		CAPSULE,
		SPHERE,
	};

	CollisionType collisionType_;
	bool isHit_ = false;
	bool isVisible_ = false;
};

class BoxCollider2D final : public ColliderComponent
{
public:
	BoxCollider2D() :
		offset_(),
		size_()
	{
		collisionType_ = CollisionType::BOX2D;
	}

	void draw(ID3D11DeviceContext* dc) override;

	void updateDebugGui(float elapsedTime) override
	{
		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::TreeNode("BoxCollider2D")) {
			ImGui::SameLine();
			ImGui::Text("          ");
			ImGui::SameLine();
			ImGui::Checkbox(u8"可視化", &isVisible_);
			ImGui::Spacing();

			ImGui::Checkbox(u8"判定結果", &isHit_);
			ImGui::DragFloat2(u8"サイズ", &size_.x, 1.0f);
			ImGui::DragFloat2(u8"オフセット", &offset_.x, 1.0f);

			ImGui::TreePop();
		}
	}


	Vector2 size_;
	Vector2 offset_;
};

class CircleCollider final : public ColliderComponent
{
public:
	CircleCollider() :
		offset_(),
		radius_()
	{
		collisionType_ = CollisionType::CIRCLE;
	}

	void draw(ID3D11DeviceContext* dc) override;

	void updateDebugGui(float elapsedTime) override
	{
		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::TreeNode("CircleCollider")) {
			ImGui::SameLine();
			ImGui::Text("          ");
			ImGui::SameLine();
			ImGui::Checkbox(u8"可視化", &isVisible_);
			ImGui::Spacing();

			ImGui::Checkbox(u8"判定結果", &isHit_);
			ImGui::DragFloat(u8"半径", &radius_, 1.0f);
			ImGui::DragFloat2(u8"オフセット", &offset_.x, 1.0f);

			ImGui::TreePop();
		}
	}


	float	radius_;
	Vector2 offset_;
};

class BoxCollider final : public ColliderComponent
{
public:
	BoxCollider() :
		size_(),
		offset_()
	{
		collisionType_ = CollisionType::BOX;
	}

	void draw(ID3D11DeviceContext* dc) override;

	void updateDebugGui(float elapsedTime) override
	{
		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::TreeNode("BoxCollider")) {
			ImGui::SameLine();
			ImGui::Text("          ");
			ImGui::SameLine();
			ImGui::Checkbox(u8"可視化", &isVisible_);
			ImGui::Spacing();

			ImGui::Checkbox(u8"判定結果", &isHit_);
			ImGui::DragFloat3(u8"サイズ", &size_.x, 0.1f);
			ImGui::DragFloat3(u8"オフセット", &offset_.x, 0.1f);

			ImGui::TreePop();
		}
	}


	Vector3 size_;
	Vector3 offset_;
};

class CapsuleCollider final : public ColliderComponent
{
public:
	CapsuleCollider() :
		radius_(),
		height_(),
		offset_()
	{
		collisionType_ = CollisionType::CAPSULE;
	}

	void draw(ID3D11DeviceContext* dc) override;

	void updateDebugGui(float elapsedTime) override
	{
		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::TreeNode("CapsuleCollider")) {
			ImGui::SameLine();
			ImGui::Text("          ");
			ImGui::SameLine();
			ImGui::Checkbox(u8"可視化", &isVisible_);
			ImGui::Spacing();

			ImGui::Checkbox(u8"判定結果", &isHit_);
			ImGui::DragFloat(u8"半径", &radius_, 0.1f);
			ImGui::DragFloat(u8"高さ", &height_, 0.1f);
			ImGui::DragFloat3(u8"オフセット", &offset_.x, 0.1f);

			ImGui::TreePop();
		}
	}


	float radius_;
	float height_;
	Vector3 offset_;
};

class SphereCollider final : public ColliderComponent
{
public:
	SphereCollider() :
		radius_(),
		offset_()
	{
		collisionType_ = CollisionType::SPHERE;
	}

	void draw(ID3D11DeviceContext* dc) override;

	void updateDebugGui(float elapsedTime) override
	{
		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::TreeNode("SphereCollider")) {
			ImGui::SameLine();
			ImGui::Text("          ");
			ImGui::SameLine();
			ImGui::Checkbox(u8"可視化", &isVisible_);
			ImGui::Spacing();

			ImGui::Checkbox(u8"判定結果", &isHit_);
			ImGui::DragFloat(u8"半径", &radius_, 0.1f);
			ImGui::DragFloat3(u8"オフセット", &offset_.x, 0.1f);

			ImGui::TreePop();
		}
	}


	float radius_;
	Vector3 offset_;
};


class PointLightComponent final : public Component
{
public:
	PointLightComponent() :
		color_(1.0f, 1.0f, 1.0f),
		range_(),
		intensity_(1.0f),
		isVisible_(),
		isOn_(true)
	{}

	void update();

	void draw(ID3D11DeviceContext* dc) override;

	void updateDebugGui(float elapsedTime) override
	{
		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::TreeNode("PointLight")) {
			ImGui::SameLine();
			ImGui::Text("          ");
			ImGui::SameLine();
			ImGui::Checkbox(u8"可視化", &isVisible_);
			ImGui::Spacing();

			ImGui::Checkbox(u8"有効化", &isOn_);
			ImGui::ColorEdit3(u8"色", &color_.x);
			ImGui::DragFloat(u8"範囲", &range_, 0.1f, 0.0f);
			ImGui::DragFloat(u8"照度", &intensity_, 0.1f, 0.0f);

			ImGui::TreePop();
		}
	}

	Vector3 color_;
	float range_;
	float intensity_;
	bool isVisible_;
	bool isOn_;
};


class CameraComponent final : public Component
{
public:
	CameraComponent() :
		viewProjection_(),
		invViewProjection_(),
		view_(),
		invView_(),
		projection_(),
		frontVec_(),
		upVec_(),
		rightVec_(),
		target_(),
		fov_(60.0f),
		nearZ_(0.1f),
		farZ_(1000.0f),
		range_(25.0f),
		isVisible_()
	{}

	void update();

	void draw(ID3D11DeviceContext* dc) override;

	void updateDebugGui(float elapsedTime) override;

	Matrix viewProjection_;
	Matrix invViewProjection_;
	Matrix view_;
	Matrix invView_;
	Matrix projection_;
	Vector3 frontVec_;
	Vector3 upVec_;
	Vector3 rightVec_;
	Vector3 target_;
	float fov_;
	float nearZ_;
	float farZ_;
	float range_;
	bool isVisible_;
};


// ===== 行動クラス =======================================================================================================================================
class Behavior
{
public:
	// --- 毎フレーム呼び出される更新処理関数 ---
	virtual void execute(GameObject* object, float elapsedTime) = 0;

	// --- オブジェクトが衝突した際に呼び出される関数 ---
	virtual void hit(GameObject* src, GameObject* dst, float elapsedTime) = 0;
};


// ===== 消去クラス =======================================================================================================================================
class Eraser
{
public:
	// --- 更新処理が終わった後に呼び出される純粋仮想関数 ---
	virtual void execute(GameObject* object) = 0;
};


// ===== ゲームオブジェクトクラス =============================================================================================================================
enum class ObjectType
{
	NONE,
	PLAYER,
	PICKAXE,
	BLOCK,
};

class GameObject
{
public:
	GameObject();

	// --- 更新処理 ---
	void update(float elapsedTime);

	// --- 削除 ---
	void destroy();

	// --- コンポーネントの追加 ---
	template<typename T> T* addComponent()
	{
		std::shared_ptr<T> component = std::make_shared<T>();
		component->setObject(this);
		components_.emplace_back(component);
		return component.get();
	}

	template<typename T> T* addCollider()
	{
		std::shared_ptr<T> component = std::make_shared<T>();
		component->setObject(this);
		components_.emplace_back(component);
		colliders_.emplace_back(component.get());
		return component.get();
	}

	// --- コンポーネントの取得 ---
	template<typename T> T* getComponent()
	{
		for (auto& component : components_)
		{
			auto castedComponent = std::dynamic_pointer_cast<T>(component);

			if (castedComponent)
				return castedComponent.get();
		}

		return nullptr;
	}

	std::string	name_;
	int			state_;
	float		timer_;
	bool		isChoose_;
	ObjectType	type_;

	Behavior*	behavior_ = nullptr;
	Eraser*		eraser_	  = nullptr;

	std::vector<std::shared_ptr<Component>> components_;
	std::vector<ColliderComponent*> colliders_;
	std::shared_ptr<Transform> transform_;

	GameObject* parent_;
	std::vector<GameObject*> child_;
};


// ===== ゲームオブジェクト管理クラス ========================================================================================================================
class GameObjectManager
{
private:
	GameObjectManager() {};
	~GameObjectManager() {};
	GameObjectManager(const GameObjectManager&) = delete;
	GameObjectManager& operator=(const GameObjectManager&) = delete;
	GameObjectManager(GameObjectManager&&) noexcept = delete;
	GameObjectManager& operator=(GameObjectManager&&) noexcept = delete;

public:
	// --- インスタンスの取得 ---
	static GameObjectManager& instance()
	{
		static GameObjectManager instance;
		return instance;
	}


	void update(float elapsedTime);
	void draw(ID3D11DeviceContext* dc);
	void judgeCollision(float elapsedTime);
	void remove();
	void updateDebugGui(float elapsedTime);
	void showDebugList();

	// --- オブジェクトの追加 ---
	GameObject* add(std::shared_ptr<GameObject> object, const Vector3& position = Vector3::Zero_, Behavior* behavior = nullptr);


	std::list<std::shared_ptr<GameObject>> objectList_;	// オブジェクトのリスト
	std::set<GameObject*> discardList_;					// 削除予定のオブジェクトのリスト
};