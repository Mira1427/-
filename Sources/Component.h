#include "GameObject.h"

// --- ���̃R���|�[�l���g ---
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

			ImGui::DragFloat3(u8"���x", &velocity_.x);
			ImGui::DragFloat3(u8"�����x", &accel_.x);
			ImGui::DragFloat3(u8"�d��", &gravity_.x);

			ImGui::TreePop();
		}
	}

	Vector3 velocity_;
	Vector3 accel_;
	Vector3 gravity_;
};


// --- �u���b�N�R���|�[�l���g ---
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

			ImGui::InputInt(u8"���C�t", &life_);

			ImGui::TreePop();
		}
	}

	int life_;
};


// --- �{�^���R���|�[�l���g ---
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

			ImGui::InputInt(u8"�C���f�b�N�X", &index_);

			ImGui::TreePop();
		}
	}


	int index_;
};