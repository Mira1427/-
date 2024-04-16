#include "Illumination.h"

#include <vector>

#include "Graphics.h"

#include "../../External/ImGui/imgui.h"

// ===== 照明管理クラス =======================================================================================================================================

// --- コンストラクタ ---
IlluminationManager::IlluminationManager(ID3D11Device* device)
{
	Graphics::createConstantBuffer(device, constantBuffer_.GetAddressOf(), sizeof(IlluminationManager::Constants));

	ambientLight_.color_ = Vector3(0.3f, 0.3f, 0.3f);

	directionLight_.direction_ = { 0.0f, 0.0f, 1.0f };
	directionLight_.intensity_ = 1.0f;
	directionLight_.color_ = { 1.0f, 1.0f, 1.0f };
}


// --- 更新処理 ---
void IlluminationManager::update(ID3D11DeviceContext* dc)
{
	IlluminationManager::Constants data;

	// --- アンビエントライト ---
	{
		data.ambientLight_.color_ = ambientLight_.color_;
	}

	// --- ディレクションライト ---
	{
		data.directionLight_.direction_ = directionLight_.direction_;
		data.directionLight_.intensity_ = directionLight_.intensity_;
		data.directionLight_.color_		= directionLight_.color_;
	}

	// --- ポイントライト ---
	{
		int index = 0;
		for (const auto& light : pointLights_)
			data.pointLights_[index++] = light;

		pointLights_.clear();
	}

	Graphics::updateConstantBuffer(dc, constantBuffer_.Get(), data);
}


// --- デバッグGui更新 ---
void IlluminationManager::updateDebugGui()
{
	static bool isUniform = false;

	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("AmbientLight"))
	{
		ImGui::ColorEdit3("Color", &ambientLight_.color_.x);

		ImGui::TreePop();
	}

	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("DirectionLight"))
	{
		ImGui::DragFloat3("Direction", &directionLight_.direction_.x, 0.01f);
		ImGui::ColorEdit3("Color", &directionLight_.color_.x);
		ImGui::DragFloat("Intensity", &directionLight_.intensity_);


		ImGui::TreePop();
	}

	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("PointLight"))
	{
		ImGui::Text("Size : %d", static_cast<int>(pointLights_.size() - 1));

		ImGui::TreePop();
	}
}
