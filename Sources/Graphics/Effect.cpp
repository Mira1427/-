#include "Effect.h"

#include "EffectManager.h"


// ===== エフェクトクラス ========================================================================================================================

// --- コンストラクタ ---
Effect::Effect(const char* fileName)
{
	// --- リソースの読み込み ---
	char16_t utf16FileName[256];
	Effekseer::ConvertUtf8ToUtf16(utf16FileName, 256, fileName);

	auto& effekseerManager = EffectManager::instance().getEffekseerManager();

	// --- エフェクトの読み込み ---
	effekseerEffect_ = Effekseer::Effect::Create(effekseerManager, (EFK_CHAR*)utf16FileName);
}


// --- 再生 ---
Effekseer::Handle Effect::play(const Vector3& position, float scale)
{
	auto& effekseerManager = EffectManager::instance().getEffekseerManager();

	Effekseer::Handle handle = effekseerManager->Play(effekseerEffect_, position.x, position.y, position.z);
	effekseerManager->SetScale(handle, scale, scale, scale);

	return handle;
}


// --- 停止 ---
void Effect::stop(Effekseer::Handle handle)
{
	auto& effekseerManager = EffectManager::instance().getEffekseerManager();

	effekseerManager->StopEffect(handle);
}


// --- 座標設定 ---
void Effect::setPosition(Effekseer::Handle handle, const Vector3& position)
{
	auto& effekseerManager = EffectManager::instance().getEffekseerManager();

	effekseerManager->SetLocation(handle, position.x, position.y, position.z);
}


// --- スケール設定 ---
void Effect::setScale(Effekseer::Handle handle, const Vector3& scale)
{
	auto& effekseerManager = EffectManager::instance().getEffekseerManager();

	effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
}
