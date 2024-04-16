#include "GameObject.h"

// --- 剛体コンポーネント ---
class RigidBodyComponent final : public Component
{
public:
	RigidBodyComponent() :
		velocity_(),
		accel_(),
		gravity_()
	{}

	void draw(ID3D11DeviceContext* dc) override {};

	void updateDebugGui(float elapsedTime) override
	{
		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::TreeNode("RigidBody")) {
			ImGui::Spacing();

			ImGui::DragFloat3(u8"速度", &velocity_.x);
			ImGui::DragFloat3(u8"加速度", &accel_.x);
			ImGui::DragFloat3(u8"重力", &gravity_.x);

			ImGui::TreePop();
		}
	}

	Vector3 velocity_;
	Vector3 accel_;
	Vector3 gravity_;
};


// --- ブロックコンポーネント ---
class BlockComponent final : public Component
{
public:
	BlockComponent() :
		life_()
	{}

	void draw(ID3D11DeviceContext* dc) override {};

	void updateDebugGui(float elapsedTime) override
	{
		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::TreeNode("RigidBody")) {
			ImGui::Spacing();

			ImGui::InputInt(u8"ライフ", &life_);

			ImGui::TreePop();
		}
	}

	int life_;
};


// --- ボタンコンポーネント ---
class ButtonComponent final : public Component
{
public:
	ButtonComponent() :
		index_()
	{}


	void draw(ID3D11DeviceContext* dc) override {};

	void updateDebugGui(float elapsedTime) override
	{
		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::TreeNode("Button")) {
			ImGui::Spacing();

			ImGui::InputInt(u8"インデックス", &index_);

			ImGui::TreePop();
		}
	}


	int index_;
};