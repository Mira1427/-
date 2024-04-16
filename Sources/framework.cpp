#include "framework.h"
#include "sceneManager.h"
#include "./Library/fileManager.h"
#include "Result.h"

#include "Graphics/FontManager.h"
#include "Graphics/EffectManager.h"

#include "Library/Audio.h"
#include "Library/Vector.h"
#include "Library/Collision.h"
#include "Library/Camera.h"

#include "BehaviorManager.h"
#include "Component.h"

std::vector<AnimationFrameData> animeList =
{
	{ 1, {0 * 300.0f, 0 * 300.0f}, {300.0f, 300.0f} },
	{ 1, {1 * 300.0f, 0 * 300.0f}, {300.0f, 300.0f} },
	{ 1, {2 * 300.0f, 0 * 300.0f}, {300.0f, 300.0f} },
	{ 1, {3 * 300.0f, 0 * 300.0f}, {300.0f, 300.0f} },
	{ 1, {4 * 300.0f, 0 * 300.0f}, {300.0f, 300.0f} },
	{ 1, {5 * 300.0f, 0 * 300.0f}, {300.0f, 300.0f} },
	{ 1, {6 * 300.0f, 0 * 300.0f}, {300.0f, 300.0f} },
	{ 1, {7 * 300.0f, 0 * 300.0f}, {300.0f, 300.0f} },
};

std::vector<AnimationFrameData> animeList2 =
{
	{ 1, {0 * 300.0f, 1 * 300.0f}, {300.0f, 300.0f} },
	{ 1, {1 * 300.0f, 1 * 300.0f}, {300.0f, 300.0f} },
	{ 1, {2 * 300.0f, 1 * 300.0f}, {300.0f, 300.0f} },
	{ 1, {3 * 300.0f, 1 * 300.0f}, {300.0f, 300.0f} },
	{ 1, {4 * 300.0f, 1 * 300.0f}, {300.0f, 300.0f} },
	{ 1, {5 * 300.0f, 1 * 300.0f}, {300.0f, 300.0f} },
	{ 1, {6 * 300.0f, 1 * 300.0f}, {300.0f, 300.0f} },
	{ 1, {7 * 300.0f, 1 * 300.0f}, {300.0f, 300.0f} },
};

std::vector<AnimationData> animeData;



Framework::Framework(HWND hwnd)
{
	window::setWindowedStyle(static_cast<DWORD>(GetWindowLongPtrA(window::getHwnd(), GWL_STYLE)));
}


bool Framework::initialize() {
	
	//	デバイス・デバイスコンテキスト・スワップチェーンの作成
	HRESULT hr{ S_OK };


	Graphics& graphics = Graphics::instance();

	graphics.initialize();


	//	定数バッファオブジェクトの生成
	{
		Graphics::createConstantBuffer(graphics.getDevice(), graphics.constantBuffers[Constants::OBJECT].GetAddressOf(), sizeof(Constants::Object));
		Graphics::createConstantBuffer(graphics.getDevice(), graphics.constantBuffers[Constants::SCENE].GetAddressOf(), sizeof(Constants::Scene));
		Graphics::createConstantBuffer(graphics.getDevice(), graphics.constantBuffers[Constants::MATERIAL].GetAddressOf(), sizeof(Constants::Material));
		Graphics::createConstantBuffer(graphics.getDevice(), graphics.constantBuffers[Constants::DISSOLVE].GetAddressOf(), sizeof(Constants::Dissolve));
		Graphics::createConstantBuffer(graphics.getDevice(), graphics.constantBuffers[Constants::WINDOW].GetAddressOf(), sizeof(Constants::Window));
		Graphics::createConstantBuffer(graphics.getDevice(), graphics.constantBuffers[Constants::SHADER_TOY].GetAddressOf(), sizeof(Constants::ShaderToy));
	}

	{
		Constants::Window data;
		data.width_ = window::getWidth();
		data.height_ = window::getHeight();
		graphics.updateConstantBuffer<Constants::Window>(Constants::WINDOW, data);

		graphics.getDeviceContext()->PSSetConstantBuffers(
			Constants::WINDOW, 1, graphics.constantBuffers[Constants::WINDOW].GetAddressOf());

		graphics.getDeviceContext()->VSSetConstantBuffers(
			Constants::WINDOW, 1, graphics.constantBuffers[Constants::WINDOW].GetAddressOf());
	}


#ifdef USE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\YuGothM.ttc", 14.0f, nullptr, glyphRangesJapanese);
	ImGui_ImplWin32_Init(window::getHwnd());
	ImGui_ImplDX11_Init(graphics.getDevice(), graphics.getDeviceContext());
	ImGui::StyleColorsDark();
#endif


#ifdef USE_IMGUI
	setImGuiStyle();
#endif

	particle_ = std::make_unique<Particle>(graphics.getDevice(), 1000);

	{
		EffectManager::instance().initialize();

		effect_ = std::make_unique<Effect>("./Data/Effect/Hit.efk");
	}

	// --- 2D関連の初期設定 ---
	{
		FontManager::instance().createTextFormat(L"メイリオ Size22.spritefont", 50);
		FontManager::instance().createColorBrush(D2D1::ColorF::WhiteSmoke);

		TextureManager::instance().load(graphics.getDevice());
	}

	// --- オブジェクトの初期設定 ---
	{
		animeData.emplace_back(AnimationData("Idle", &*TextureManager::instance().getTexture(0), &animeList));
		animeData.emplace_back(AnimationData("Death", &*TextureManager::instance().getTexture(0), &animeList2));


		// --- 2Dオブジェクト ---
		//{
		//	auto* obj = GameObjectManager::instance().add(std::make_shared<GameObject>(), Vector3(), &BehaviorManager::spriteBehavior_);

		//	auto* animation = obj->addComponent<AnimationComponent>();
		//	animation->animationClips_ = &animeData;

		//	obj->addComponent<SpriteRendererComponent>();
		//}
		//{
		//	auto* obj = GameObjectManager::instance().add(std::make_shared<GameObject>(), Vector3(), &BehaviorManager::spriteBehavior_);

		//	auto* animation = obj->addComponent<AnimationComponent>();
		//	animation->animationClips_ = &animeData;

		//	obj->addComponent<SpriteRendererComponent>();
		//}
	}


	//	入力処理用
	InputManager::instance().initialize();


	// --- シーン関連の初期設定 ---
	{
		SceneManager::instance().nextScene = &Title::instance();	// 開始シーンの設定
	}


	// --- オーディオ関連の初期設定 ---
	{
		music::load("./Data/Musics/title.wav");
		sound::load("./Data/Sounds/shot.wav");
	}


	// --- シェーダー関連の初期化 ---
	{
		frameBuffers[0] = std::make_unique<FrameBuffer>(graphics.getDevice(), window::getWidthInt(), window::getHeightInt());
		frameBuffers[1] = std::make_unique<FrameBuffer>(graphics.getDevice(), window::getWidthInt() * 0.5f, window::getHeightInt() * 0.5f);

		bitBlockTransfer = std::make_unique<FullScreenQuad>(graphics.getDevice());

		Graphics::createPSFromCSO(graphics.getDevice(), "./Data/Shaders/LuminanceExtractionPS.cso", pixelShaders[0].GetAddressOf());
		Graphics::createPSFromCSO(graphics.getDevice(), "./Data/Shaders/BlurPS.cso", pixelShaders[1].GetAddressOf());
		Graphics::createPSFromCSO(graphics.getDevice(), "./Data/Shaders/ShaderToySamplePS.cso", pixelShaders[2].GetAddressOf());

		shaderToy_ = std::make_unique<ShaderToy>(graphics.getDevice());
	}

	return true;
}


void Framework::update(float elapsedTime/*Elapsed seconds from last frame*/) {

	elapsedTime = (std::min)(elapsedTime, 1.0f /60.0f);

	//	次のシーンが現在と異なるなら
	if (SceneManager::instance().scene != SceneManager::instance().nextScene) {

		//	現在のシーンの終了処理
		if(SceneManager::instance().scene) 
			SceneManager::instance().scene->deinitialize();

		//	シーンを変更して初期設定
		SceneManager::instance().scene = SceneManager::instance().nextScene;
		SceneManager::instance().scene->initialize();
	}


	//	入力処理の更新
	input::update();

	// --- ウィンドウを閉じる ---
	if (GetAsyncKeyState(VK_ESCAPE))
		window::close();

	// --- 1280 * 720 ---
	if (GetAsyncKeyState(VK_F1))
	{
		SetWindowPos(window::getHwnd(), NULL, 0, 0, 1280, 720, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	// --- 1920 * 1080 ---
	if (GetAsyncKeyState(VK_F2))
	{
		SetWindowPos(window::getHwnd(), NULL, 0, 0, 1920, 1080, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	// --- フルスクリーン ---
	//if (GetAsyncKeyState(VK_MENU) & 1)
	//{
	//	window::stylize(!window::isFullScreen());
	//}


#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif

#ifdef USE_IMGUI
	//	初期位置とサイズ
	ImGui::SetNextWindowPos(ImVec2(40, 40), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(400, 1020), ImGuiSetCond_Once);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 10.0f);

	//	本体
	ImGui::Begin(u8"ImGui", nullptr, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar()) {

		if (ImGui::BeginMenu(u8"シーン切り替え")) {

			if (ImGui::MenuItem(u8"タイトル")) {

				SceneManager::instance().nextScene = &Title::instance();
			}

			if (ImGui::MenuItem(u8"ゲーム")) {

				SceneManager::instance().nextScene = &Game::instance();
			}

			if (ImGui::MenuItem(u8"リザルト")) {

				SceneManager::instance().nextScene = &Result::instance();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}


	if (ImGui::BeginTabBar(u8"a")) {

		if (ImGui::BeginTabItem(u8"システム")) {

			//	フレームレート表示
			ImGui::BulletText("%.1f FPS", ImGui::GetIO().Framerate);

			ImGui::Text(u8"ウィンドウのサイズ x:%f y:%f", window::getWidth(), window::getHeight());
			ImGui::Text(u8"カーソルの座標 x:%d, y:%d", input::getCursorPosX(), input::getCursorPosY());

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"照明関係"))
		{
			Graphics::instance().getIlluminationManager()->updateDebugGui();

			ImGui::EndTabItem();
		}

		TextureManager::instance().updateDebugGui();


		if (ImGui::BeginTabItem(u8"オーディオ")) {

			static float masterVolume = 1.0f;
			static float musicVolume = 1.0f;
			static float musicMasterVolume = 1.0f;
			static float soundVolume = 1.0f;
			static float soundMasterVolume = 1.0f;

			auto& audio = AudioManager::instance();

			ImGui::Text(u8"音楽の音量:%f", music::getFinalVolume(0));
			ImGui::Text(u8"効果音の音量:%f", sound::getFinalVolume(0));

			ImGui::SliderFloat(u8"マスターボリューム", &masterVolume, 0.0f, 1.0f);
			audio::setVolume(masterVolume);
			ImGui::SliderFloat(u8"音楽の音量", &musicVolume, 0.0f, 1.0f);
			music::setVolume(0, musicVolume);
			ImGui::SliderFloat(u8"音楽全体の音量", &musicMasterVolume, 0.0f, 1.0f);
			music::setMasterVolume(musicMasterVolume);
			ImGui::SliderFloat(u8"効果音の音量", &soundVolume, 0.0f, 1.0f);
			sound::setVolume(0, soundVolume);
			ImGui::SliderFloat(u8"効果音全体の音量", &soundMasterVolume, 0.0f, 1.0f);
			sound::setMasterVolume(soundMasterVolume);

			if (ImGui::Button(u8"音楽再生", ImVec2(200, 50)))
			{
				music::play(0);
			}

			if (ImGui::Button(u8"音楽停止", ImVec2(200, 50)))
			{
				music::stop(0);
			}

			if (ImGui::Button(u8"効果音再生", ImVec2(200, 50)))
			{
				sound::play(0);
			}

			if (ImGui::Button(u8"効果音停止", ImVec2(200, 50)))
			{
				sound::play(0);
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	if (ImGui::Button("Particle", ImVec2(200, 30)))
	{
		particle_->initialize(Graphics::instance().getDeviceContext(), 0);
	}

	if (ImGui::Button("Effect", ImVec2(200, 30)))
	{
		effect_->play({ 0, 0, 0 });
	}

	if (ImGui::Button(u8"ブロック追加", ImVec2(200.0f, 30.0f)))
	{
		{
			auto* obj = GameObjectManager::instance().add(std::make_shared<GameObject>(), Vector3(10.0f, 0.0f, 0.0f), &BehaviorManager::blockBehavior_);
			{
				obj->name_ = u8"ブロック";
				obj->type_ = ObjectType::BLOCK;

				obj->addCollider<BoxCollider>();
				obj->addComponent<BlockComponent>();

				auto* staticMeshRenderer = obj->addComponent<StaticMeshRendererComponent>();
				staticMeshRenderer->model_ = Game::instance().cube_;
			}
		}
	}

	ImGui::End();

	particle_->update(Graphics::instance().getDeviceContext(), elapsedTime);


	SceneManager::instance().scene->ImGui();

	GameObjectManager::instance().showDebugList();
	GameObjectManager::instance().updateDebugGui(elapsedTime);

#endif

	Camera::instance().update(elapsedTime);
	
	SceneManager::instance().scene->update(elapsedTime);

	Graphics::instance().getIlluminationManager()->update(Graphics::instance().getDeviceContext());
}


void Framework::render(float elapsedTime/*Elapsed seconds from last frame*/)
{
	Graphics& graphics = Graphics::instance();

	ID3D11RenderTargetView* nullRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
	graphics.getDeviceContext()->OMSetRenderTargets(_countof(nullRenderTargetViews), nullRenderTargetViews, 0);
	ID3D11ShaderResourceView* nullShaderResourceViews[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	graphics.getDeviceContext()->VSSetShaderResources(0, _countof(nullShaderResourceViews), nullShaderResourceViews);
	graphics.getDeviceContext()->PSSetShaderResources(0, _countof(nullShaderResourceViews), nullShaderResourceViews);

	HRESULT hr{ S_OK };

	renderTarget::set();

	sampler::set();

	//	深度ステンシルステートの設定
	graphics.setDepthStencil(true, true);

	//	ラスタライザステートの設定
	rasterize::set(rasterize::NONE);

	//	ブレンディングステートオブジェクトの設定
	blend::set(blend::ALPHA);


	// --- シェーダートイの定数バッファ更新 ---
	{
		Constants::ShaderToy data;
		data.iTime_ = tictoc.time_stamp();
		data.iResolution_ = { window::getSize().x, window::getSize().y, 0.0f, 0.0f };
		data.iMouse_ = { input::getCursorPos().x, input::getCursorPos().y, 0.0f, 0.0f };
		graphics.updateConstantBuffer<Constants::ShaderToy>(Constants::SHADER_TOY, data);

		graphics.getDeviceContext()->PSSetConstantBuffers(
			Constants::SHADER_TOY, 1, graphics.constantBuffers[Constants::SHADER_TOY].GetAddressOf()
		);


		//frameBuffers[0]->clear(graphics.getDeviceContext());
		//frameBuffers[0]->active(graphics.getDeviceContext());

		//ID3D11ShaderResourceView* shaderResourceViews[4]{};
		//shaderToy_->blit(graphics.getDeviceContext(), tictoc.time_stamp(), shaderResourceViews, 0, 4, pixelShaders[2].Get());
		// 
		//frameBuffers[0]->deactive(graphics.getDeviceContext());
	}

	{
		frameBuffers[0]->clear(graphics.getDeviceContext());
		frameBuffers[0]->active(graphics.getDeviceContext());

		SceneManager::instance().scene->render(graphics.getDeviceContext());
		{
			blend::set(blend::ADD);
			rasterize::set(rasterize::CULL_BACK);
			Graphics::instance().setDepthStencil(true, true);
			particle_->draw(graphics.getDeviceContext());
		}

		frameBuffers[0]->deactive(graphics.getDeviceContext());
	}



	blend::set(blend::ALPHA);
	rasterize::set(rasterize::NONE);
	graphics.setDepthStencil(true, true);

	frameBuffers[1]->clear(graphics.getDeviceContext());
	frameBuffers[1]->active(graphics.getDeviceContext());
	bitBlockTransfer->blit(graphics.getDeviceContext(), frameBuffers[0]->shaderResourceViews[0].GetAddressOf(), 0, 1, pixelShaders[0].Get());
	frameBuffers[1]->deactive(graphics.getDeviceContext());


	ID3D11ShaderResourceView* shaderResourceViews[2]
	{
		frameBuffers[0]->shaderResourceViews[0].Get(),
		frameBuffers[1]->shaderResourceViews[0].Get()
	};

	bitBlockTransfer->blit(graphics.getDeviceContext(), shaderResourceViews, 0, 2, pixelShaders[1].Get());



	//ID3D11ShaderResourceView* shaderResourceViews[4]{};
	//shaderToy_->blit(graphics.getDeviceContext(), tictoc.time_stamp(),
	//	shaderResourceViews, 0, 4, pixelShaders[2].Get());




#ifdef USE_IMGUI
	ImGui::PopStyleVar(3);

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	//UINT syncInterval{ (window::getVsync() ? 1U : 0U) };
	//UINT flags = (window::isTearingSupported() && !window::isFullScreen() && !window::getVsync()) ? DXGI_PRESENT_ALLOW_TEARING : 0;
	//graphics.getSwapChain()->Present(syncInterval, flags);

	UINT syncInterval = 0U;
	graphics.getSwapChain()->Present(syncInterval, 0);

	InputManager::instance().m_.wheel_ = 0;
}


bool Framework::uninitialize() {

	return true;
}


Framework::~Framework() {
	
	SceneManager::instance().scene->deinitialize();

	// デバッグインターフェースを取得
	ID3D11Debug* pDebug = nullptr;
	Graphics::instance().getDevice()->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&pDebug));

	if (pDebug)
	{
		// InfoQueue インターフェースを取得
		ID3D11InfoQueue* pInfoQueue = nullptr;
		pDebug->QueryInterface(__uuidof(ID3D11InfoQueue), reinterpret_cast<void**>(&pInfoQueue));

		if (pInfoQueue)
		{
			// メッセージの出力レベルを設定
			pInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, TRUE);

			// ライブオブジェクトのレポートを有効にする
			D3D11_MESSAGE_ID hide[] = {
				D3D11_MESSAGE_ID_DEVICE_DRAW_CONSTANT_BUFFER_TOO_SMALL,    // 特定のメッセージは除外することもできます
			};
			D3D11_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			pInfoQueue->AddStorageFilterEntries(&filter);

			// ライブオブジェクトの状態を確認
			pInfoQueue->Release();
		}

		pDebug->Release();
	}
}


#ifdef USE_IMGUI
void Framework::setImGuiStyle() {

	auto& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text]					= ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_TextDisabled]			= ImVec4{ 0.50f, 0.50f, 0.50f, 0.50f };
	style.Colors[ImGuiCol_WindowBg]				= ImVec4{ 0.30f, 0.30f, 0.30f, 0.30f };
	style.Colors[ImGuiCol_ChildBg]				= ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_PopupBg]				= ImVec4{ 0.08f, 0.08f, 0.08f, 1.00f };
	style.Colors[ImGuiCol_Border]				= ImVec4{ 0.70f, 0.70f, 1.00f, 0.00f };
	style.Colors[ImGuiCol_BorderShadow]			= ImVec4{ 0.00f, 0.00f, 0.00f, 0.00f };
	style.Colors[ImGuiCol_FrameBg]				= ImVec4{ 0.00f, 0.00f, 0.00f, 0.30f };
	style.Colors[ImGuiCol_FrameBgHovered]		= ImVec4{ 1.00f, 1.00f, 1.00f, 0.25f };
	style.Colors[ImGuiCol_FrameBgActive]		= ImVec4{ 0.00f, 0.00f, 0.00f, 0.50f };
	style.Colors[ImGuiCol_TitleBg]				= ImVec4{ 0.00f, 0.00f, 0.00f, 0.50f };
	style.Colors[ImGuiCol_TitleBgActive]		= ImVec4{ 0.00f, 0.00f, 0.00f, 1.00f };
	style.Colors[ImGuiCol_TitleBgCollapsed]		= ImVec4{ 0.00f, 0.00f, 0.00f, 0.50f };
	style.Colors[ImGuiCol_MenuBarBg]			= ImVec4{ 0.10f, 0.10f, 0.10f, 0.80f };
	style.Colors[ImGuiCol_ScrollbarBg]			= ImVec4{ 0.50f, 0.50f, 0.50f, 0.50f };
	style.Colors[ImGuiCol_ScrollbarGrab]		= ImVec4{ 1.00f, 1.00f, 1.00f, 0.30f };
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.40f, 0.40f, 0.40f, 1.00f };
	style.Colors[ImGuiCol_ScrollbarGrabActive]	= ImVec4{ 1.00f, 1.00f, 1.00f, 0.80f };
	style.Colors[ImGuiCol_CheckMark]			= ImVec4{ 0.26f, 0.59f, 0.98f, 1.00f };
	style.Colors[ImGuiCol_SliderGrab]			= ImVec4{ 1.00f, 1.00f, 1.00f, 0.40f };
	style.Colors[ImGuiCol_SliderGrabActive]		= ImVec4{ 1.00f, 1.00f, 1.00f, 0.70f };
	style.Colors[ImGuiCol_Button]				= ImVec4{ 1.00f, 1.00f, 1.00f, 0.15f };
	style.Colors[ImGuiCol_ButtonHovered]		= ImVec4{ 1.00f, 1.00f, 1.00f, 0.30f };
	style.Colors[ImGuiCol_ButtonActive]			= ImVec4{ 1.00f, 1.00f, 1.00f, 0.50f };
	style.Colors[ImGuiCol_Header]				= ImVec4{ 0.30f, 0.60f, 1.00f, 0.50f };
	style.Colors[ImGuiCol_HeaderHovered]		= ImVec4{ 1.00f, 1.00f, 1.00f, 0.15f };
	style.Colors[ImGuiCol_HeaderActive]			= ImVec4{ 1.00f, 1.00f, 1.00f, 0.30f };
	style.Colors[ImGuiCol_Separator]			= ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_SeparatorHovered]		= ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_SeparatorActive]		= ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_ResizeGrip]			= ImVec4{ 1.00f, 1.00f, 1.00f, 0.25f };
	style.Colors[ImGuiCol_ResizeGripHovered]	= ImVec4{ 1.00f, 1.00f, 1.00f, 0.60f };
	style.Colors[ImGuiCol_ResizeGripActive]		= ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_Tab]					= ImVec4{ 0.00f, 0.00f, 0.00f, 0.30f };
	style.Colors[ImGuiCol_TabHovered]			= ImVec4{ 0.40f, 0.40f, 0.40f, 0.60f };
	style.Colors[ImGuiCol_TabActive]			= ImVec4{ 0.50f, 0.50f, 0.50f, 1.00f };
	style.Colors[ImGuiCol_TabUnfocused]			= ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_TabUnfocusedActive]	= ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_PlotLines]			= ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_PlotLinesHovered]		= ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_PlotHistogram]		= ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_TextSelectedBg]		= ImVec4{ 0.50f, 0.50f, 0.50f, 0.60f };
	style.Colors[ImGuiCol_DragDropTarget]		= ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
}

void Framework::ImGuiStyleEditor() {

	static DirectX::XMFLOAT4 colors[ImGuiCol_COUNT];
	const char* names[ImGuiCol_COUNT] = {
		u8"テキスト",
		u8"テキスト無効時",
		u8"ウィンドウ背景",
		u8"子背景",
		u8"ポップアップ背景",
		u8"ボーダー",
		u8"ボーダーの影",
		u8"フレームの背景",
		u8"フレームの浮いてるとき",
		u8"フレームのアクティブなとき",
		u8"タイトルの背景",
		u8"タイトルのアクティブなとき",
		u8"タイトルのたたんでる時",
		u8"メニューの背景",
		u8"スクロールの背景",
		u8"スクロール掴んでる時",
		u8"スクロール掴んで浮いてるとき",
		u8"スクロール掴んでアクティブな時",
		u8"チェックマークの色",
		u8"スライダー掴んでる時",
		u8"スライダー掴んでアクティブなとき",
		u8"ボタンの色",
		u8"ボタン浮いてるとき",
		u8"ボタンアクティブなとき",
		u8"ヘッダーの色",
		u8"ヘッダーの浮いてるとき",
		u8"ヘッダーアクティブなとき",
		u8"セパレーターの色",
		u8"セパレーター浮いてるとき",
		u8"セパレーターアクティブなとき",
		u8"リサイズ掴んでる時",
		u8"リサイズ掴んで浮いてるとき",
		u8"リサイズ掴んでアクティブなとき",
		u8"タブの色",
		u8"タブ浮いてるとき",
		u8"タブアクティブなとき",
		u8"タブフォーカスしてないとき",
		u8"タブアクティブでフォーカスしてないとき",
		u8"ラインの色",
		u8"ライン浮いてるとき",
		u8"ヒストグラムの色",
		u8"ヒストグラム浮いてるとき",
		u8"テキスト選択の色",
		u8"ドラッグドロップターゲット",
		u8"直前に選ばれたやつの色",
		u8"直前にctrl + tab押した奴の色",
		u8"分からん01",
		u8"分からん02"
	};

	auto& style = ImGui::GetStyle();

	char* name = 0;

	for (int i = 0; i < ImGuiCol_COUNT; i++) {

		//ImGui::SliderFloat4(names[i], &colors[i].x, 0, 1.0f);
		ImGui::SliderFloat4(names[i], &style.Colors[i].x, 0, 1.0f);
	}

	style.Colors[ImGuiCol_Text]					=	ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_TextDisabled]			=	ImVec4{ 0.50f, 0.50f, 0.50f, 0.50f };
	style.Colors[ImGuiCol_WindowBg]				=	ImVec4{ 0.30f, 0.30f, 0.30f, 0.80f };
	style.Colors[ImGuiCol_ChildBg]				=	ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_PopupBg]				=	ImVec4{ 0.08f, 0.08f, 0.08f, 1.00f };
	style.Colors[ImGuiCol_Border]				=	ImVec4{ 0.70f, 0.70f, 1.00f, 0.00f };
	style.Colors[ImGuiCol_BorderShadow]			=	ImVec4{ 0.00f, 0.00f, 0.00f, 0.00f };
	style.Colors[ImGuiCol_FrameBg]				=	ImVec4{ 0.00f, 0.00f, 0.00f, 0.25f };
	style.Colors[ImGuiCol_FrameBgHovered]		=	ImVec4{ 1.00f, 1.00f, 1.00f, 0.25f };
	style.Colors[ImGuiCol_FrameBgActive]		=	ImVec4{ 0.00f, 0.00f, 0.00f, 0.50f };
	style.Colors[ImGuiCol_TitleBg]				=	ImVec4{ 0.00f, 0.00f, 0.00f, 0.50f };
	style.Colors[ImGuiCol_TitleBgActive]		=	ImVec4{ 0.00f, 0.00f, 0.00f, 1.00f };
	style.Colors[ImGuiCol_TitleBgCollapsed]		=	ImVec4{ 0.00f, 0.00f, 0.00f, 0.50f };
	style.Colors[ImGuiCol_MenuBarBg]			=	ImVec4{ 0.10f, 0.10f, 0.10f, 0.80f };
	style.Colors[ImGuiCol_ScrollbarBg]			=	ImVec4{ 0.50f, 0.50f, 0.50f, 0.50f };
	style.Colors[ImGuiCol_ScrollbarGrab]		=	ImVec4{ 1.00f, 1.00f, 1.00f, 0.30f };
	style.Colors[ImGuiCol_ScrollbarGrabHovered]	=	ImVec4{ 0.40f, 0.40f, 0.40f, 1.00f };
	style.Colors[ImGuiCol_ScrollbarGrabActive]	=	ImVec4{ 1.00f, 1.00f, 1.00f, 0.80f };
	style.Colors[ImGuiCol_CheckMark]			=	ImVec4{ 0.26f, 0.59f, 0.98f, 1.00f };
	style.Colors[ImGuiCol_SliderGrab]			=	ImVec4{ 1.00f, 1.00f, 1.00f, 0.40f };
	style.Colors[ImGuiCol_SliderGrabActive]		=	ImVec4{ 1.00f, 1.00f, 1.00f, 0.70f };
	style.Colors[ImGuiCol_Button]				=	ImVec4{ 1.00f, 1.00f, 1.00f, 0.15f };
	style.Colors[ImGuiCol_ButtonHovered]		=	ImVec4{ 1.00f, 1.00f, 1.00f, 0.30f };
	style.Colors[ImGuiCol_ButtonActive]			=	ImVec4{ 1.00f, 1.00f, 1.00f, 0.50f };
	style.Colors[ImGuiCol_Header]				=	ImVec4{ 1.00f, 1.00f, 1.00f, 0.30f };
	style.Colors[ImGuiCol_HeaderHovered]		=	ImVec4{ 0.00f, 1.00f, 0.00f, 0.30f };
	style.Colors[ImGuiCol_HeaderActive]			=	ImVec4{ 1.00f, 1.00f, 1.00f, 0.30f };
	style.Colors[ImGuiCol_Separator]			=	ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_SeparatorHovered]		=	ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_SeparatorActive]		=	ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_ResizeGrip]			=	ImVec4{ 1.00f, 1.00f, 1.00f, 0.25f };
	style.Colors[ImGuiCol_ResizeGripHovered]	=	ImVec4{ 1.00f, 1.00f, 1.00f, 0.60f };
	style.Colors[ImGuiCol_ResizeGripActive]		=	ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_Tab]					=	ImVec4{ 0.00f, 0.00f, 0.00f, 0.30f };
	style.Colors[ImGuiCol_TabHovered]			=	ImVec4{ 0.40f, 0.40f, 0.40f, 0.60f };
	style.Colors[ImGuiCol_TabActive]			=	ImVec4{ 0.50f, 0.50f, 0.50f, 1.00f };
	style.Colors[ImGuiCol_TabUnfocused]			=	ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_TabUnfocusedActive]	=	ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_PlotLines]			=	ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_PlotLinesHovered]		=	ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_PlotHistogram]		=	ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_PlotHistogramHovered]	=	ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_TextSelectedBg]		=	ImVec4{ 0.50f, 0.50f, 0.50f, 0.60f };
	style.Colors[ImGuiCol_DragDropTarget]		=	ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };

	ImGui::Spacing();
	ImGui::Separator();

	static int myInt;
	static float myFloat;
	static bool myBool;
	ImGui::InputInt(u8"インプット", &myInt);
	ImGui::SliderFloat(u8"スライダー", &myFloat, 0, 1);
	ImGui::DragFloat(u8"ドラッグ", &myFloat, 0.1f, 0, 1);
	ImGui::Checkbox(u8"チェックボックス", &myBool);
	if (ImGui::Button(u8"ボタン", ImVec2(200, 25))) {

		ImGui::OpenPopup(u8"ポップアップ");
	}

	if (ImGui::BeginPopup(u8"ポップアップ")) {

		ImGui::BulletText(u8"ああああああああああああああああああああああああああああああああああああああああああああ");
		ImGui::EndPopup();
	}

	if (ImGui::BeginMenuBar()) {

		if (ImGui::BeginMenu(u8"メニュー")) {

			if (ImGui::MenuItem(u8"メニューアイテム")) {


			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}


	//if (ImGui::BeginChild(u8"child", ImVec2(200, 25), true)) {

	//	ImGui::BulletText(u8"ああああああああああああああああああああああああああああああああああああああああああああ");

	//	ImGui::EndChild();
	//}


	if (ImGui::BeginTabBar(u8"タブ")) {

		if (ImGui::BeginTabItem(u8"タブアイテム")) {

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(u8"タブアイテム2")) {

			ImGui::EndTabItem();
		}


		if (ImGui::BeginTabItem(u8"タブアイテム3")) {

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}
#endif
