#include "EffectManager.h"

#include "Graphics.h"


// --- 初期化 ---
void EffectManager::initialize()
{
	Graphics& graphics = Graphics::instance();

	// --- レンダラー生成 ---
	effekseerRenderer_ = EffekseerRendererDX11::Renderer::Create(graphics.getDevice(), graphics.getDeviceContext(), 2048);

	// --- マネージャー生成 ---
	effekseerManager_ = Effekseer::Manager::Create(2048);

	// --- レンダラーの各種設定 ---
	effekseerManager_->SetSpriteRenderer(effekseerRenderer_->CreateSpriteRenderer());
	effekseerManager_->SetRibbonRenderer(effekseerRenderer_->CreateRibbonRenderer());
	effekseerManager_->SetRingRenderer(effekseerRenderer_->CreateRingRenderer());
	effekseerManager_->SetTrackRenderer(effekseerRenderer_->CreateTrackRenderer());
	effekseerManager_->SetModelRenderer(effekseerRenderer_->CreateModelRenderer());

	// --- ローダーの設定 ---
	effekseerManager_->SetTextureLoader(effekseerRenderer_->CreateTextureLoader());
	effekseerManager_->SetModelLoader(effekseerRenderer_->CreateModelLoader());
	effekseerManager_->SetMaterialLoader(effekseerRenderer_->CreateMaterialLoader());

	// --- 左手座標系で計算 ---
	effekseerManager_->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}


// --- 更新処理 ---
void EffectManager::update(float elapsedTime)
{
	effekseerManager_->Update(elapsedTime * 60.0f);
}


// --- 描画処理 ---
void EffectManager::render(const Matrix& view, const Matrix& projection)
{
	// --- カメラの情報を設定 ---
	effekseerRenderer_->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
	effekseerRenderer_->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

	// --- 描画処理 ---
	effekseerRenderer_->BeginRendering();
	effekseerManager_->Draw();
	effekseerRenderer_->EndRendering();
}
