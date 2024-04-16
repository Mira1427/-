#pragma once

#include <Effekseer.h>

#include "../Library/Matrix.h"


// ===== �G�t�F�N�g�N���X ========================================================================================================================
class Effect
{
public:
	Effect(const char* fileName);
	~Effect() {}

	// --- �Đ� ---
	Effekseer::Handle play(const Vector3& position, float scale = 1.0f);

	// --- ��~ ---
	void stop(Effekseer::Handle handle);

	// --- ���W�ݒ� ---
	void setPosition(Effekseer::Handle handle, const Vector3& position);

	// --- �X�P�[���ݒ� ---
	void setScale(Effekseer::Handle handle, const Vector3& scale);

private:
	Effekseer::EffectRef effekseerEffect_;
};

