#pragma once

#include <list>

#include <d3d11.h>
#include <wrl.h>

#include "../Library/Vector.h"

class IlluminationManager
{
public:
	IlluminationManager(ID3D11Device* device);

	void update(ID3D11DeviceContext* dc);

	void updateDebugGui();

	struct AmbientLight
	{
		Vector3 color_;
		float pad_;
	};

	struct DirectionLight
	{
		Vector3 direction_;
		float	intensity_;
		Vector3 color_;
		float pad2_;
	};

	inline static constexpr int POINT_LIGHT_MAX = 256;
	struct PointLight
	{
		Vector3 position_;
		float intensity_;
		Vector3 color_;
		float range_;
	};

	struct Constants
	{
		AmbientLight	ambientLight_;
		DirectionLight	directionLight_;
		PointLight	pointLights_[POINT_LIGHT_MAX];
	};


	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer_;	// 定数バッファ

	AmbientLight	ambientLight_;		// アンビエントライト
	DirectionLight	directionLight_;	// ディレクションライト
	std::list<PointLight> pointLights_;	// ポイントライト
	PointLight pointLight_;
};

