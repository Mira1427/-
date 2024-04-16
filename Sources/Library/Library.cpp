#include "library.h"
#include "math.h"
#include "fileManager.h"
#include "Audio.h"
#include "../Graphics/Graphics.h"


//========================================
//			ウィンドウ関連
//========================================

LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

namespace window {

	struct WindowData
	{
		LONG width_, height_;
		BOOL isFullScreen_ = FALSE;
		HWND hwnd_;
		DWORD windowedStyle_;
		BOOL vsync_ = TRUE;
		BOOL tearingSupported_ = FALSE;
	}data;

	HWND getHwnd() {

		return data.hwnd_;
	}

	BOOL getVsync()
	{
		return data.vsync_;
	}

	BOOL isTearingSupported()
	{
		return data.tearingSupported_;
	}

	void setTdearingSupport(BOOL flag)
	{
		data.tearingSupported_ = flag;
	}

	void initialize(_In_ HINSTANCE instance, _In_ int cmd_show) {

#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		//_CrtSetBreakAlloc(####);
#endif


#if 0
		//	ウィンドウの情報をファイルから取得
		FileManager::load(std::make_unique<WindowModeFile>());

#else

#if 1
		data.width_ = 1920;
		data.height_ = 1080;
#else 
		data.width_ = 1280;
		data.height_ = 720;
#endif

#endif

		WNDCLASSEXW wcex{};
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = window_procedure;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = instance;
		wcex.hIcon = 0;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = APPLICATION_NAME;
		wcex.hIconSm = 0;
		RegisterClassExW(&wcex);

		RECT rc{ 0, 0, data.width_, data.height_ };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
		data.hwnd_ = CreateWindowExW(
			0,
			APPLICATION_NAME,
			L"A",
			WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
			rc.right - rc.left,
			rc.bottom - rc.top,
			NULL,
			NULL,
			instance,
			NULL
		);
		ShowWindow(data.hwnd_, cmd_show);

	}

	const DirectX::XMFLOAT2 getSize() {

		return static_cast<DirectX::XMFLOAT2>(DirectX::XMFLOAT2(data.width_, data.height_));
	}

	const DirectX::XMFLOAT2 getCenter() {

		return getSize() * 0.5f;
	}

	const float getWidth() {

		return static_cast<float>(data.width_);
	}

	const int getWidthInt() {

		return static_cast<int>(data.width_);
	}

	const float getHeight() {

		return static_cast<float>(data.height_);
	}

	const int getHeightInt() {

		return static_cast<int>(data.height_);
	}

	const bool isFullScreen() {

		return data.isFullScreen_;
	}

	DWORD getWindowedStyle()
	{
		return data.windowedStyle_;
	}

	void setWindowedStyle(DWORD style)
	{
		data.windowedStyle_ = style;
	}

	//	画面の拡大率
	const DirectX::XMFLOAT2 getWindowScaling() {

		return window::getSize() / DirectX::XMFLOAT2(1920.0f, 1080.0f);
	}

	//	背景のクリア
	void clear(const DirectX::XMFLOAT4& color) {

		Graphics::instance().clearRenderTargetView(color);

		Graphics::instance().clearDepthStencilView();
	}


	void close() {

		PostMessage(data.hwnd_, WM_CLOSE, 0, 0);
	}

	void onSizeChanged(UINT64 width, UINT64 height) {

		HRESULT hr{ S_OK };
		if (width != data.width_ || height != data.height_) {

			Graphics& graphics = Graphics::instance();

			data.width_ = static_cast<LONG>(width);
			data.height_ = static_cast<LONG>(height);

			graphics.resetD2D1DeviceContext();

			Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory6;
			hr = graphics.getSwapChain()->GetParent(IID_PPV_ARGS(dxgiFactory6.GetAddressOf()));
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			graphics.createSwapChain(dxgiFactory6.Get());
			graphics.createDepthStencilView();
			graphics.createViewport();

			graphics.createDirect2DObjects();
		}
	}


	void stylize(BOOL fullScreen)
	{
		data.isFullScreen_ = fullScreen;
		if (fullScreen)
		{
			RECT windowedRect;
			GetWindowRect(data.hwnd_, &windowedRect);
			SetWindowLongPtrA(
				data.hwnd_,
				GWL_STYLE,
				WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME)
			);

			RECT fullScreenWindowRect;
			HRESULT hr{ E_FAIL };
			auto& graphics = Graphics::instance();

			if (graphics.getSwapChain())
			{
				Microsoft::WRL::ComPtr<IDXGIOutput> dxgiOutput;
				hr = graphics.getSwapChain()->GetContainingOutput(&dxgiOutput);
				if (hr == S_OK)
				{
					DXGI_OUTPUT_DESC outputDesc;
					hr = dxgiOutput->GetDesc(&outputDesc);
					if (hr == S_OK)
					{
						fullScreenWindowRect = outputDesc.DesktopCoordinates;
					}
				}
			}

			if (hr != S_OK)
			{
				DEVMODE devMode = {};
				devMode.dmSize = sizeof(DEVMODE);
				EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);

				fullScreenWindowRect = {
					devMode.dmPosition.x,
					devMode.dmPosition.y,
					devMode.dmPosition.x + static_cast<LONG>(devMode.dmPelsWidth),
					devMode.dmPosition.y + static_cast<LONG>(devMode.dmPelsHeight)
				};
			}

			SetWindowPos(
				data.hwnd_,
#ifdef _DEBUG
				NULL,
#else
				HWND_TOPMOST,
#endif
				fullScreenWindowRect.left,
				fullScreenWindowRect.top,
				fullScreenWindowRect.right,
				fullScreenWindowRect.bottom,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			ShowWindow(data.hwnd_, SW_MAXIMIZE);
		}

		else
		{
			DWORD windowedStyle = window::getWindowedStyle();
			SetWindowLongPtrA(data.hwnd_, GWL_STYLE, window::getWindowedStyle());
			SetWindowPos(
				data.hwnd_,
				HWND_NOTOPMOST,
				0,
				0,
				window::getWidth(),
				window::getHeight(),
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			ShowWindow(data.hwnd_, SW_NORMAL);
		}
	}
}



//=====================================
//			入力関連
//=====================================
namespace input {

	//---------------------------
	//		入力の初期設定
	//---------------------------
	void initialize() {

		InputManager::instance().initialize();
	}

	//---------------------------
	//		入力の更新処理
	//---------------------------
	void update() {

		InputManager::instance().update();
	}


	//-------------------------------
	//		キーを押している間
	//-------------------------------
	DirectX::Keyboard::State keyPressed() {

		return InputManager::instance().getKeyState();
	}

	//-------------------------------
	//		キーを押した瞬間
	//-------------------------------
	DirectX::Keyboard::State keyDown() {

		return InputManager::instance().isKeyDown();
	}

	//-------------------------------
	//		キーを離した瞬間
	//-------------------------------
	DirectX::Keyboard::State keyUp() {

		return InputManager::instance().isKeyUp();
	}


	//========================================
	//			ゲームパッド関連
	//========================================
	DirectX::GamePad::State getState() {

		return InputManager::instance().gamePad_->GetState(0);
	}


	//	接続されているかどうか
	bool isConnected(const int& padNum) {

		return InputManager::instance().isConnected(padNum);
	}


	//	左スティックの傾き ( -1.0 ~ 1.0 )
	DirectX::XMFLOAT2 getLeft() {

		return DirectX::XMFLOAT2(InputManager::instance().p_.stick_.leftx, InputManager::instance().p_.stick_.leftY_);
	}

	float getLeftX() {

		return InputManager::instance().p_.stick_.leftx;
	}

	float getLeftY() {

		return InputManager::instance().p_.stick_.leftY_;
	}


	//	デッドゾーン込み
	float getLeftX(float deadZoneValue) {

		return std::abs(InputManager::instance().p_.stick_.leftx) > deadZoneValue ? InputManager::instance().p_.stick_.leftx : 0.0f;
	}

	float getLeftY(float deadZoneValue) {

		return std::abs(InputManager::instance().p_.stick_.leftY_) > deadZoneValue ? InputManager::instance().p_.stick_.leftY_ : 0.0f;
	}

	DirectX::XMFLOAT2 getLeft(float deadZoneValue) {

		return DirectX::XMFLOAT2(getLeftX(deadZoneValue), getLeftY(deadZoneValue));
	}


	//	デットゾーン関連
	void setLeftStickDeadZone(float value) {

		InputManager::instance().p_.stick_.leftDeadZone_ = value;
	}

	float getLeftStickDeadZone() {

		return InputManager::instance().p_.stick_.leftDeadZone_;
	}


	//	右スティックの傾き ( -1.0 ~ 1.0 )
	DirectX::XMFLOAT2 getRight() {

		return DirectX::XMFLOAT2(InputManager::instance().p_.stick_.rightx, InputManager::instance().p_.stick_.rightY_);
	}

	float getRightX() {

		return InputManager::instance().p_.stick_.rightx;
	}

	float getRightY() {

		return InputManager::instance().p_.stick_.rightY_;
	}


	//	デッドゾーン込み
	float getRightX(float deadZoneValue) {

		return std::abs(InputManager::instance().p_.stick_.rightx) > deadZoneValue ? InputManager::instance().p_.stick_.rightx : 0.0f;
	}

	float getRightY(float deadZoneValue) {

		return std::abs(InputManager::instance().p_.stick_.rightY_) > deadZoneValue ? InputManager::instance().p_.stick_.rightY_ : 0.0f;
	}

	DirectX::XMFLOAT2 getRight(float deadZoneValue) {

		return DirectX::XMFLOAT2(getRightX(deadZoneValue), getRightY(deadZoneValue));
	}

	//	デットゾーン関連
	float getRightStickDeadZone() {

		return InputManager::instance().p_.stick_.rightDeadZone_;
	}

	void setRightStickDeadZone(float value) {

		InputManager::instance().p_.stick_.rightDeadZone_ = value;
	}



	//	左トリガー
	float getLeftTrigger() {

		return InputManager::instance().p_.trigger_.left_;
	}

	//	デッドゾーン込み
	float getLeftTrigger(float deadZoneValue) {

		return InputManager::instance().p_.trigger_.left_ > deadZoneValue ? InputManager::instance().p_.trigger_.left_ : 0.0f;
	}

	float getLeftTriggerDeadZone() {

		return InputManager::instance().p_.trigger_.leftDeadZone_;
	}


	//	右トリガー
	float getRightTrigger() {

		return InputManager::instance().p_.trigger_.right_;
	}

	//	デッドゾーン込み
	float getRightTrigger(float deadZoneValue) {

		return InputManager::instance().p_.trigger_.right_ > deadZoneValue ? InputManager::instance().p_.trigger_.right_ : 0.0f;
	}

	float getRightTriggerDeadZone() {

		return InputManager::instance().p_.trigger_.rightDeadZone_;
	}


	//	デッドゾーンの設定
	void setLeftTriggerDeadZone(float value) {

		InputManager::instance().p_.trigger_.leftDeadZone_ = value;
	}

	void setRightTriggerDeadZone(float value) {

		InputManager::instance().p_.trigger_.rightDeadZone_ = value;
	}


	//========================================
	//			入力にビットをたてる
	//========================================
	
	//--------------------------
	//		長押し
	//--------------------------
	uint64_t state() {

		return InputManager::instance().state();
	}

	uint64_t state(const uint64_t& keyLabel)
	{
		return InputManager::instance().state() & keyLabel;
	}

	//------------------------------
	//		押した瞬間
	//------------------------------
	uint64_t trigger() {

		return InputManager::instance().down();
	}

	uint64_t trigger(const uint64_t& keyLabel) {

		return InputManager::instance().down() & keyLabel;
	}

	//------------------------------
	//		離した瞬間
	//------------------------------
	uint64_t release() {

		return InputManager::instance().up();
	}

	uint64_t release(const uint64_t& keyLabel) {

		return InputManager::instance().up() & keyLabel;
	}

	//--------------------------------------------
	//		カーソルの座標を取得
	//--------------------------------------------
	DirectX::XMFLOAT2 getCursorPos() {

		return InputManager::instance().getCursorPos();
	}

	int getCursorPosX() {

		return InputManager::instance().getCursorPosX();
	}

	int getCursorPosY() {

		return InputManager::instance().getCursorPosY();
	}

	const bool cursorIsMoved() {

		return InputManager::instance().m_.isMove_;
	}

	// --- カーソルの変化量の取得 ---
	int getCursorDeltaX()
	{
		return InputManager::instance().getCursorDeltaX();
	}
	int getCursorDeltaY()
	{
		return InputManager::instance().getCursorDeltaY();
	}

	int getMouseWheel()
	{
		return InputManager::instance().m_.wheel_;
	}
}



// ===== オーディオ関連 ==================================================================================================================================
namespace audio
{
	// --- インスタンスの取得 ---
	AudioManager& get()
	{
		return AudioManager::instance();
	}

	// --- 全体の音量の取得 ---
	float getVolume()
	{
		return get().getMasterVolume();
	}

	// --- 全体の音量の設定 ---
	void setVolume(float volume)
	{
		get().setMasterVolume(volume);
	}
}



// ===== 音楽関連 ============================================================================================================================================
namespace music
{
	// --- 最終的な音量の取得 ---
	// - 引数 : 音楽のインデックス
	// - 戻り値 : 音量 × 音楽全体の音量 × 全体の音量 の結果
	float getFinalVolume(int index)
	{
		return audio::get().getMusicFinalVolume(index);
	}


	// --- 音楽全体の音量の取得 ---
	float getMasterVolume()
	{
		return audio::get().getMusicMasterVolume();
	}

	// --- 音楽全体の音量の設定 ---
	void setMasterVolume(float volume)
	{
		audio::get().setMusicMasterVolume(volume);
	}


	// --- 音量の取得 ---
	float getVolume(int index)
	{
		return audio::get().getMusicVolume(index);
	}

	// --- 音量の設定 ---
	void setVolume(int index, float volume)
	{
		audio::get().setMusicVolume(index, volume);
	}


	// --- 読み込み ---
	void load(const char* fileName, float volume)
	{
		audio::get().loadMusic(fileName, volume);
	}

	// --- 再生 ---
	void play(int index)
	{
		audio::get().playMusic(index);
	}

	// --- 停止 ---
	void stop(int index)
	{
		audio::get().stopMusic(index);
	}

	// --- 一括停止 ---
	void stop()
	{
		audio::get().stopMusic();
	}
}



// ===== 効果音関連 =======================================================================================================================================
namespace sound
{
	// --- 最終的な音量の取得 ---
	// - 戻り値 : 音量 × 音楽全体の音量 × 全体の音量 の結果
	float getFinalVolume(int index)
	{
		return audio::get().getSoundFinalVolume(index);
	}


	// --- 全体の音量の取得 ---
	float getMasterVolume()
	{
		return audio::get().getSoundMasterVolume();
	}

	// --- 全体の音量の設定 ---
	void setMasterVolume(float volume)
	{
		audio::get().setSoundMasterVolume(volume);
	}


	// --- 音量の取得 ---
	float getVolume(int index)
	{
		return audio::get().getSoundVolume(index);
	}

	// --- 音量の設定 ---
	void setVolume(int index, float volume)
	{
		audio::get().setSoundVolume(index, volume);
	}


	// --- 読み込み ---
	void load(const char* fileName, float volume)
	{
		audio::get().loadSound(fileName, volume);
	}

	// --- 再生 ---
	void play(int index)
	{
		audio::get().playSound(index);
	}

	// --- 停止 ---
	void stop(int index)
	{
		audio::get().stopSound(index);
	}

	// --- 一括停止 ---
	void stop()
	{
		audio::get().stopSound();
	}
}




//=====================================
//			数学関連
//=====================================
namespace math {

	//=============================================
	//			値の制限と範囲外の確認
	//=============================================
	
	//--------------------------------
	//		値の制限
	//--------------------------------
	float clamp(const float& v, const float& min, const float& max) {

		if (v > max || v < min) {

			if (v > max) { return max; }
			if (v < min) { return min; }
		}

		return v;
	}

	//--------------------------------
	//		値一つとの比較
	//--------------------------------
	float clamp(const float& v1, const char* mark, const float& v2) {

		//	min
		if (mark == "<") {

			if (v1 < v2) 
				return v2;

			//return Math::clampMin(v1, v2);
		}

		//	max
		else if (mark == ">") {

			if (v1 > v2) 
			return v2;

			//return Math::clampMax(v1, v2);
		}

		else {

			assert(!"不等号がありません");
		}

		return v1;
	}


	//------------------------------
	//		範囲外の確認
	//------------------------------
	bool isOutOfRange(const float& v, const float& min, const float& max) {

		return Math::isOutOfRange(v, min, max);
	}

	//--------------------------------
	//		> で max  < で min
	//--------------------------------
	float isOutOfRange(const float& v1, const char* mark, const float& v2) {

		if (mark == "<") {

			return Math::isOutOfRangeMin(v1, v2);
		}

		else if (mark == ">") {

			return Math::isOutOfRangeMax(v1, v2);
		}

		else {

			assert(!"不等号がありません");
		}

		return -1.0f;
	}



	//========================================
	//			ランダムな値を生成
	//========================================
	int rand(const int& range, const int& offset) {

		return Math::rand(range, offset);
	}


	float frand(const float& max, const float& min) {

		return Math::frand(max, min);
	}


	//===============================================================================================
	//			小数関連
	//===============================================================================================

	//	四捨五入
	int round(float num, int decimalPlace) {

		return static_cast<int>(num + 5.0f / (10 * decimalPlace));
	}

	//	小数切り上げ
	int roundUp(float num) {

		return static_cast<int>(num + 0.9999f);
	}




	//===================================
	//			ベクトル関連
	//===================================
	void normalize(DirectX::XMFLOAT2& v) {

		return Math::normalize(v);
	}

	// --- ベクトルの計算 ---
	DirectX::XMFLOAT2 calcVector(
		const DirectX::XMFLOAT2& src,	// 元
		const DirectX::XMFLOAT2& dst,	// 目標
		bool isNormalize				// 正規化の有無
	) {

		DirectX::XMFLOAT2 result{};

		result = dst - src;

		if (isNormalize) {	// 正規化

			DirectX::XMStoreFloat2(&result, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&result)));
		}

		return result;
	}
}


//=============================================
//			オーバーロード演算子
//=============================================

//		足し算
DirectX::XMFLOAT2 operator+(const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2) {

	return DirectX::XMFLOAT2(v1.x + v2.x, v1.y + v2.y);
}

DirectX::XMFLOAT2 operator+=(DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2) {

	return DirectX::XMFLOAT2(v1.x += v2.x, v1.y += v2.y);
}


//		引き算
DirectX::XMFLOAT2 operator-(const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2) {

	return DirectX::XMFLOAT2(v1.x - v2.x, v1.y - v2.y);
}

DirectX::XMFLOAT2 operator-=(DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2) {

	return DirectX::XMFLOAT2(v1.x -= v2.x, v1.y -= v2.y);
}


//		掛け算
DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2) {

	return DirectX::XMFLOAT2(v1.x * v2.x, v1.y * v2.y);
}

DirectX::XMFLOAT2 operator*=(DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2) {

	return DirectX::XMFLOAT2(v1.x *= v2.x, v1.y *= v2.y);
}

DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& v1, const float& v2) {

	return DirectX::XMFLOAT2(v1.x * v2, v1.y * v2);
}


//		割り算
DirectX::XMFLOAT2 operator/(const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2) {

	return DirectX::XMFLOAT2(v1.x / v2.x, v1.y / v2.y);
}

DirectX::XMFLOAT2 operator/=(DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2) {

	return DirectX::XMFLOAT2(v1.x /= v2.x, v1.y /= v2.y);
}



//==========================================================================================
//			イージング
//==========================================================================================
namespace easing {

	EasingFunction getFunction(EasingFunctions function) {

		return getEasingFunction(function);
	}


	//	実体ver
	bool calculate(Data& data, float& target, float increaseAmount, float elapsedTime) {

		//	反転
		if (data.isReverse_) {

			data.incrementingTimer_ = 0;

			double progress = data.function_(data.decrementingTimer_ / static_cast<float>(data.decrementingTimerLimit_));
			target = static_cast<float>(progress * increaseAmount);

			if ((data.decrementingTimer_ -= elapsedTime) < 0) {

				data.decrementingTimer_ = 0;
				return true;	//	タイマーが下限に達した場合、完了として true を返す
			}

		}

		else {

			data.decrementingTimer_ = data.decrementingTimerLimit_;

			double progress = data.function_(data.incrementingTimer_ / static_cast<float>(data.incrementingTimerLimit_));
			target = static_cast<float>(progress * increaseAmount);

			if ((data.incrementingTimer_ += elapsedTime) > data.incrementingTimerLimit_) {

				data.incrementingTimer_ = data.incrementingTimerLimit_;
				return true;	//	タイマーが上限に達した場合、完了として true を返す
			}
		}

		return false;	//	まだ進行中の場合は false を返す
	}


	//	ポインタver
	bool calculate(std::shared_ptr<Data> data, float& target, float increaseAmount, float elapsedTime) {

		//	反転
		if (data->isReverse_) {

			data->incrementingTimer_ = 0;

			double progress = data->function_(data->decrementingTimer_ / static_cast<float>(data->decrementingTimerLimit_));
			target = static_cast<float>(progress * increaseAmount);

			if ((data->decrementingTimer_ -= elapsedTime) < 0) {

				data->decrementingTimer_ = 0;
				return true;	//	タイマーが下限に達した場合、完了として true を返す
			}
		}

		else {

			data->decrementingTimer_ = data->decrementingTimerLimit_;

			double progress = data->function_(data->incrementingTimer_ / static_cast<float>(data->incrementingTimerLimit_));
			target = static_cast<float>(progress * increaseAmount);

			if ((data->incrementingTimer_ += elapsedTime) > data->incrementingTimerLimit_) {

				data->incrementingTimer_ = data->incrementingTimerLimit_;
				return true;	//	タイマーが上限に達した場合、完了として true を返す
			}
		}

		return false;	//	まだ進行中の場合は false を返す
	}
}




//========================================
//			深度ステンシル用
//========================================
namespace depthStencil {

	void depthStencil::set(const int& state) {

		Graphics::instance().setDepthStencil(state);
	}
}



//========================================
//			サンプラー用
//========================================
namespace sampler {

	void set() {

		Graphics::instance().setSamplerState();
	}
}



//========================================
//			ブレンド関連
//========================================
namespace blend {

	void set(int state) {

		Graphics::instance().setBlendState(state);
	}
}



//========================================
//			ラスタライザ関連
//========================================
namespace rasterize {

	void set(int state) {

		Graphics::instance().setRasterizeState(state);
	}
}



//====================================================================================================
//			レンダーターゲット関連
//====================================================================================================
void renderTarget::set() {

	Graphics::instance().setRenderTarget();
}
