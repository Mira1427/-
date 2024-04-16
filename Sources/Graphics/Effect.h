#pragma once

#include <Effekseer.h>

#include "../Library/Matrix.h"


// ===== エフェクトクラス ========================================================================================================================
class Effect
{
public:
	Effect(const char* fileName);
	~Effect() {}

	// --- 再生 ---
	Effekseer::Handle play(const Vector3& position, float scale = 1.0f);

	// --- 停止 ---
	void stop(Effekseer::Handle handle);

	// --- 座標設定 ---
	void setPosition(Effekseer::Handle handle, const Vector3& position);

	// --- スケール設定 ---
	void setScale(Effekseer::Handle handle, const Vector3& scale);

private:
	Effekseer::EffectRef effekseerEffect_;
};

