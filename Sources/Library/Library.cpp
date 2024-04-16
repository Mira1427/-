#include "library.h"
#include "math.h"
#include "fileManager.h"
#include "Audio.h"
#include "../Graphics/Graphics.h"


//========================================
//			�E�B���h�E�֘A
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
		//	�E�B���h�E�̏����t�@�C������擾
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

	//	��ʂ̊g�嗦
	const DirectX::XMFLOAT2 getWindowScaling() {

		return window::getSize() / DirectX::XMFLOAT2(1920.0f, 1080.0f);
	}

	//	�w�i�̃N���A
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
//			���͊֘A
//=====================================
namespace input {

	//---------------------------
	//		���͂̏����ݒ�
	//---------------------------
	void initialize() {

		InputManager::instance().initialize();
	}

	//---------------------------
	//		���͂̍X�V����
	//---------------------------
	void update() {

		InputManager::instance().update();
	}


	//-------------------------------
	//		�L�[�������Ă����
	//-------------------------------
	DirectX::Keyboard::State keyPressed() {

		return InputManager::instance().getKeyState();
	}

	//-------------------------------
	//		�L�[���������u��
	//-------------------------------
	DirectX::Keyboard::State keyDown() {

		return InputManager::instance().isKeyDown();
	}

	//-------------------------------
	//		�L�[�𗣂����u��
	//-------------------------------
	DirectX::Keyboard::State keyUp() {

		return InputManager::instance().isKeyUp();
	}


	//========================================
	//			�Q�[���p�b�h�֘A
	//========================================
	DirectX::GamePad::State getState() {

		return InputManager::instance().gamePad_->GetState(0);
	}


	//	�ڑ�����Ă��邩�ǂ���
	bool isConnected(const int& padNum) {

		return InputManager::instance().isConnected(padNum);
	}


	//	���X�e�B�b�N�̌X�� ( -1.0 ~ 1.0 )
	DirectX::XMFLOAT2 getLeft() {

		return DirectX::XMFLOAT2(InputManager::instance().p_.stick_.leftx, InputManager::instance().p_.stick_.leftY_);
	}

	float getLeftX() {

		return InputManager::instance().p_.stick_.leftx;
	}

	float getLeftY() {

		return InputManager::instance().p_.stick_.leftY_;
	}


	//	�f�b�h�]�[������
	float getLeftX(float deadZoneValue) {

		return std::abs(InputManager::instance().p_.stick_.leftx) > deadZoneValue ? InputManager::instance().p_.stick_.leftx : 0.0f;
	}

	float getLeftY(float deadZoneValue) {

		return std::abs(InputManager::instance().p_.stick_.leftY_) > deadZoneValue ? InputManager::instance().p_.stick_.leftY_ : 0.0f;
	}

	DirectX::XMFLOAT2 getLeft(float deadZoneValue) {

		return DirectX::XMFLOAT2(getLeftX(deadZoneValue), getLeftY(deadZoneValue));
	}


	//	�f�b�g�]�[���֘A
	void setLeftStickDeadZone(float value) {

		InputManager::instance().p_.stick_.leftDeadZone_ = value;
	}

	float getLeftStickDeadZone() {

		return InputManager::instance().p_.stick_.leftDeadZone_;
	}


	//	�E�X�e�B�b�N�̌X�� ( -1.0 ~ 1.0 )
	DirectX::XMFLOAT2 getRight() {

		return DirectX::XMFLOAT2(InputManager::instance().p_.stick_.rightx, InputManager::instance().p_.stick_.rightY_);
	}

	float getRightX() {

		return InputManager::instance().p_.stick_.rightx;
	}

	float getRightY() {

		return InputManager::instance().p_.stick_.rightY_;
	}


	//	�f�b�h�]�[������
	float getRightX(float deadZoneValue) {

		return std::abs(InputManager::instance().p_.stick_.rightx) > deadZoneValue ? InputManager::instance().p_.stick_.rightx : 0.0f;
	}

	float getRightY(float deadZoneValue) {

		return std::abs(InputManager::instance().p_.stick_.rightY_) > deadZoneValue ? InputManager::instance().p_.stick_.rightY_ : 0.0f;
	}

	DirectX::XMFLOAT2 getRight(float deadZoneValue) {

		return DirectX::XMFLOAT2(getRightX(deadZoneValue), getRightY(deadZoneValue));
	}

	//	�f�b�g�]�[���֘A
	float getRightStickDeadZone() {

		return InputManager::instance().p_.stick_.rightDeadZone_;
	}

	void setRightStickDeadZone(float value) {

		InputManager::instance().p_.stick_.rightDeadZone_ = value;
	}



	//	���g���K�[
	float getLeftTrigger() {

		return InputManager::instance().p_.trigger_.left_;
	}

	//	�f�b�h�]�[������
	float getLeftTrigger(float deadZoneValue) {

		return InputManager::instance().p_.trigger_.left_ > deadZoneValue ? InputManager::instance().p_.trigger_.left_ : 0.0f;
	}

	float getLeftTriggerDeadZone() {

		return InputManager::instance().p_.trigger_.leftDeadZone_;
	}


	//	�E�g���K�[
	float getRightTrigger() {

		return InputManager::instance().p_.trigger_.right_;
	}

	//	�f�b�h�]�[������
	float getRightTrigger(float deadZoneValue) {

		return InputManager::instance().p_.trigger_.right_ > deadZoneValue ? InputManager::instance().p_.trigger_.right_ : 0.0f;
	}

	float getRightTriggerDeadZone() {

		return InputManager::instance().p_.trigger_.rightDeadZone_;
	}


	//	�f�b�h�]�[���̐ݒ�
	void setLeftTriggerDeadZone(float value) {

		InputManager::instance().p_.trigger_.leftDeadZone_ = value;
	}

	void setRightTriggerDeadZone(float value) {

		InputManager::instance().p_.trigger_.rightDeadZone_ = value;
	}


	//========================================
	//			���͂Ƀr�b�g�����Ă�
	//========================================
	
	//--------------------------
	//		������
	//--------------------------
	uint64_t state() {

		return InputManager::instance().state();
	}

	uint64_t state(const uint64_t& keyLabel)
	{
		return InputManager::instance().state() & keyLabel;
	}

	//------------------------------
	//		�������u��
	//------------------------------
	uint64_t trigger() {

		return InputManager::instance().down();
	}

	uint64_t trigger(const uint64_t& keyLabel) {

		return InputManager::instance().down() & keyLabel;
	}

	//------------------------------
	//		�������u��
	//------------------------------
	uint64_t release() {

		return InputManager::instance().up();
	}

	uint64_t release(const uint64_t& keyLabel) {

		return InputManager::instance().up() & keyLabel;
	}

	//--------------------------------------------
	//		�J�[�\���̍��W���擾
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

	// --- �J�[�\���̕ω��ʂ̎擾 ---
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



// ===== �I�[�f�B�I�֘A ==================================================================================================================================
namespace audio
{
	// --- �C���X�^���X�̎擾 ---
	AudioManager& get()
	{
		return AudioManager::instance();
	}

	// --- �S�̂̉��ʂ̎擾 ---
	float getVolume()
	{
		return get().getMasterVolume();
	}

	// --- �S�̂̉��ʂ̐ݒ� ---
	void setVolume(float volume)
	{
		get().setMasterVolume(volume);
	}
}



// ===== ���y�֘A ============================================================================================================================================
namespace music
{
	// --- �ŏI�I�ȉ��ʂ̎擾 ---
	// - ���� : ���y�̃C���f�b�N�X
	// - �߂�l : ���� �~ ���y�S�̂̉��� �~ �S�̂̉��� �̌���
	float getFinalVolume(int index)
	{
		return audio::get().getMusicFinalVolume(index);
	}


	// --- ���y�S�̂̉��ʂ̎擾 ---
	float getMasterVolume()
	{
		return audio::get().getMusicMasterVolume();
	}

	// --- ���y�S�̂̉��ʂ̐ݒ� ---
	void setMasterVolume(float volume)
	{
		audio::get().setMusicMasterVolume(volume);
	}


	// --- ���ʂ̎擾 ---
	float getVolume(int index)
	{
		return audio::get().getMusicVolume(index);
	}

	// --- ���ʂ̐ݒ� ---
	void setVolume(int index, float volume)
	{
		audio::get().setMusicVolume(index, volume);
	}


	// --- �ǂݍ��� ---
	void load(const char* fileName, float volume)
	{
		audio::get().loadMusic(fileName, volume);
	}

	// --- �Đ� ---
	void play(int index)
	{
		audio::get().playMusic(index);
	}

	// --- ��~ ---
	void stop(int index)
	{
		audio::get().stopMusic(index);
	}

	// --- �ꊇ��~ ---
	void stop()
	{
		audio::get().stopMusic();
	}
}



// ===== ���ʉ��֘A =======================================================================================================================================
namespace sound
{
	// --- �ŏI�I�ȉ��ʂ̎擾 ---
	// - �߂�l : ���� �~ ���y�S�̂̉��� �~ �S�̂̉��� �̌���
	float getFinalVolume(int index)
	{
		return audio::get().getSoundFinalVolume(index);
	}


	// --- �S�̂̉��ʂ̎擾 ---
	float getMasterVolume()
	{
		return audio::get().getSoundMasterVolume();
	}

	// --- �S�̂̉��ʂ̐ݒ� ---
	void setMasterVolume(float volume)
	{
		audio::get().setSoundMasterVolume(volume);
	}


	// --- ���ʂ̎擾 ---
	float getVolume(int index)
	{
		return audio::get().getSoundVolume(index);
	}

	// --- ���ʂ̐ݒ� ---
	void setVolume(int index, float volume)
	{
		audio::get().setSoundVolume(index, volume);
	}


	// --- �ǂݍ��� ---
	void load(const char* fileName, float volume)
	{
		audio::get().loadSound(fileName, volume);
	}

	// --- �Đ� ---
	void play(int index)
	{
		audio::get().playSound(index);
	}

	// --- ��~ ---
	void stop(int index)
	{
		audio::get().stopSound(index);
	}

	// --- �ꊇ��~ ---
	void stop()
	{
		audio::get().stopSound();
	}
}




//=====================================
//			���w�֘A
//=====================================
namespace math {

	//=============================================
	//			�l�̐����Ɣ͈͊O�̊m�F
	//=============================================
	
	//--------------------------------
	//		�l�̐���
	//--------------------------------
	float clamp(const float& v, const float& min, const float& max) {

		if (v > max || v < min) {

			if (v > max) { return max; }
			if (v < min) { return min; }
		}

		return v;
	}

	//--------------------------------
	//		�l��Ƃ̔�r
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

			assert(!"�s����������܂���");
		}

		return v1;
	}


	//------------------------------
	//		�͈͊O�̊m�F
	//------------------------------
	bool isOutOfRange(const float& v, const float& min, const float& max) {

		return Math::isOutOfRange(v, min, max);
	}

	//--------------------------------
	//		> �� max  < �� min
	//--------------------------------
	float isOutOfRange(const float& v1, const char* mark, const float& v2) {

		if (mark == "<") {

			return Math::isOutOfRangeMin(v1, v2);
		}

		else if (mark == ">") {

			return Math::isOutOfRangeMax(v1, v2);
		}

		else {

			assert(!"�s����������܂���");
		}

		return -1.0f;
	}



	//========================================
	//			�����_���Ȓl�𐶐�
	//========================================
	int rand(const int& range, const int& offset) {

		return Math::rand(range, offset);
	}


	float frand(const float& max, const float& min) {

		return Math::frand(max, min);
	}


	//===============================================================================================
	//			�����֘A
	//===============================================================================================

	//	�l�̌ܓ�
	int round(float num, int decimalPlace) {

		return static_cast<int>(num + 5.0f / (10 * decimalPlace));
	}

	//	�����؂�グ
	int roundUp(float num) {

		return static_cast<int>(num + 0.9999f);
	}




	//===================================
	//			�x�N�g���֘A
	//===================================
	void normalize(DirectX::XMFLOAT2& v) {

		return Math::normalize(v);
	}

	// --- �x�N�g���̌v�Z ---
	DirectX::XMFLOAT2 calcVector(
		const DirectX::XMFLOAT2& src,	// ��
		const DirectX::XMFLOAT2& dst,	// �ڕW
		bool isNormalize				// ���K���̗L��
	) {

		DirectX::XMFLOAT2 result{};

		result = dst - src;

		if (isNormalize) {	// ���K��

			DirectX::XMStoreFloat2(&result, DirectX::XMVector2Normalize(DirectX::XMLoadFloat2(&result)));
		}

		return result;
	}
}


//=============================================
//			�I�[�o�[���[�h���Z�q
//=============================================

//		�����Z
DirectX::XMFLOAT2 operator+(const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2) {

	return DirectX::XMFLOAT2(v1.x + v2.x, v1.y + v2.y);
}

DirectX::XMFLOAT2 operator+=(DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2) {

	return DirectX::XMFLOAT2(v1.x += v2.x, v1.y += v2.y);
}


//		�����Z
DirectX::XMFLOAT2 operator-(const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2) {

	return DirectX::XMFLOAT2(v1.x - v2.x, v1.y - v2.y);
}

DirectX::XMFLOAT2 operator-=(DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2) {

	return DirectX::XMFLOAT2(v1.x -= v2.x, v1.y -= v2.y);
}


//		�|���Z
DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2) {

	return DirectX::XMFLOAT2(v1.x * v2.x, v1.y * v2.y);
}

DirectX::XMFLOAT2 operator*=(DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2) {

	return DirectX::XMFLOAT2(v1.x *= v2.x, v1.y *= v2.y);
}

DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& v1, const float& v2) {

	return DirectX::XMFLOAT2(v1.x * v2, v1.y * v2);
}


//		����Z
DirectX::XMFLOAT2 operator/(const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2) {

	return DirectX::XMFLOAT2(v1.x / v2.x, v1.y / v2.y);
}

DirectX::XMFLOAT2 operator/=(DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2) {

	return DirectX::XMFLOAT2(v1.x /= v2.x, v1.y /= v2.y);
}



//==========================================================================================
//			�C�[�W���O
//==========================================================================================
namespace easing {

	EasingFunction getFunction(EasingFunctions function) {

		return getEasingFunction(function);
	}


	//	����ver
	bool calculate(Data& data, float& target, float increaseAmount, float elapsedTime) {

		//	���]
		if (data.isReverse_) {

			data.incrementingTimer_ = 0;

			double progress = data.function_(data.decrementingTimer_ / static_cast<float>(data.decrementingTimerLimit_));
			target = static_cast<float>(progress * increaseAmount);

			if ((data.decrementingTimer_ -= elapsedTime) < 0) {

				data.decrementingTimer_ = 0;
				return true;	//	�^�C�}�[�������ɒB�����ꍇ�A�����Ƃ��� true ��Ԃ�
			}

		}

		else {

			data.decrementingTimer_ = data.decrementingTimerLimit_;

			double progress = data.function_(data.incrementingTimer_ / static_cast<float>(data.incrementingTimerLimit_));
			target = static_cast<float>(progress * increaseAmount);

			if ((data.incrementingTimer_ += elapsedTime) > data.incrementingTimerLimit_) {

				data.incrementingTimer_ = data.incrementingTimerLimit_;
				return true;	//	�^�C�}�[������ɒB�����ꍇ�A�����Ƃ��� true ��Ԃ�
			}
		}

		return false;	//	�܂��i�s���̏ꍇ�� false ��Ԃ�
	}


	//	�|�C���^ver
	bool calculate(std::shared_ptr<Data> data, float& target, float increaseAmount, float elapsedTime) {

		//	���]
		if (data->isReverse_) {

			data->incrementingTimer_ = 0;

			double progress = data->function_(data->decrementingTimer_ / static_cast<float>(data->decrementingTimerLimit_));
			target = static_cast<float>(progress * increaseAmount);

			if ((data->decrementingTimer_ -= elapsedTime) < 0) {

				data->decrementingTimer_ = 0;
				return true;	//	�^�C�}�[�������ɒB�����ꍇ�A�����Ƃ��� true ��Ԃ�
			}
		}

		else {

			data->decrementingTimer_ = data->decrementingTimerLimit_;

			double progress = data->function_(data->incrementingTimer_ / static_cast<float>(data->incrementingTimerLimit_));
			target = static_cast<float>(progress * increaseAmount);

			if ((data->incrementingTimer_ += elapsedTime) > data->incrementingTimerLimit_) {

				data->incrementingTimer_ = data->incrementingTimerLimit_;
				return true;	//	�^�C�}�[������ɒB�����ꍇ�A�����Ƃ��� true ��Ԃ�
			}
		}

		return false;	//	�܂��i�s���̏ꍇ�� false ��Ԃ�
	}
}




//========================================
//			�[�x�X�e���V���p
//========================================
namespace depthStencil {

	void depthStencil::set(const int& state) {

		Graphics::instance().setDepthStencil(state);
	}
}



//========================================
//			�T���v���[�p
//========================================
namespace sampler {

	void set() {

		Graphics::instance().setSamplerState();
	}
}



//========================================
//			�u�����h�֘A
//========================================
namespace blend {

	void set(int state) {

		Graphics::instance().setBlendState(state);
	}
}



//========================================
//			���X�^���C�U�֘A
//========================================
namespace rasterize {

	void set(int state) {

		Graphics::instance().setRasterizeState(state);
	}
}



//====================================================================================================
//			�����_�[�^�[�Q�b�g�֘A
//====================================================================================================
void renderTarget::set() {

	Graphics::instance().setRenderTarget();
}
