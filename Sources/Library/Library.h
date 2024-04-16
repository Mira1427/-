#pragma once

#include "../Graphics/primitive.h"
#include "inputManager.h"
#include "easing.h"
#include "Vector.h"


//====================================================================================================
//			�E�B���h�E�֘A
//====================================================================================================
namespace window {

	HWND getHwnd();

	BOOL getVsync();

	BOOL isTearingSupported();

	void setTdearingSupport(BOOL flag);

	void initialize(_In_ HINSTANCE instance, _In_ int cmd_show);

	const DirectX::XMFLOAT2 getSize();

	const DirectX::XMFLOAT2 getCenter();

	const float getWidth();
	const int getWidthInt();
	
	const float getHeight();
	const int getHeightInt();

	const DirectX::XMFLOAT2 getWindowScaling();

	const bool isFullScreen();

	DWORD getWindowedStyle();
	void setWindowedStyle(DWORD style);

	//	�w�i�̃N���A
	void clear(const DirectX::XMFLOAT4& color);

	void close();

	void onSizeChanged(UINT64 width, UINT64 height);

	void stylize(BOOL fullScreen);
}



//====================================================================================================
//			���͊֘A
//====================================================================================================
namespace input {

	//--------------------------
	//		���͂̏����ݒ�
	//--------------------------
	void initialize();

	//--------------------------
	//		���͂̍X�V����
	//--------------------------
	void update();

	//==================================================
	//			�L�[����
	//==================================================
	
	//--------------------------------------------
	//		�L�[�������Ă����
	//--------------------------------------------
	DirectX::Keyboard::State keyPressed();

	//--------------------------------------------
	//		�L�[���������u��
	//--------------------------------------------
	DirectX::Keyboard::State keyDown();

	//--------------------------------------------
	//		�L�[�𗣂����u��
	//--------------------------------------------
	DirectX::Keyboard::State keyUp();


	//========================================
	//			�Q�[���p�b�h�֘A
	//========================================
	DirectX::GamePad::State getState();

	bool isConnected(const int& padNum);


	//	���X�e�B�b�N
	float getLeftX();
	float getLeftX(float deadZoneValue);
	float getLeftY();
	float getLeftY(float deadZoneValue);
	DirectX::XMFLOAT2 getLeft();
	DirectX::XMFLOAT2 getLeft(float deadZoneValue);
	float getLeftStickDeadZone();
	void setLeftStickDeadZone(float value);

	//	�E�X�e�B�b�N
	float getRightX();
	float getRightX(float deadZoneValue);
	float getRightY();
	float getRightY(float deadZoneValue);
	DirectX::XMFLOAT2 getRight();
	DirectX::XMFLOAT2 getRight(float deadZoneValue);
	float getRightStickDeadZone();
	void setRightStickDeadZone(float value);

	//	���g���K�[
	float getLeftTrigger();
	float getLeftTrigger(float deadZoneValue);
	float getLeftTriggerDeadZone();
	void  setLeftTriggerDeadZone(float value);

	//	�E�g���K�[
	float getRightTrigger();
	float getRightTrigger(float deadZoneValue);
	float getRightTriggerDeadZone();
	void  setRightTriggerDeadZone(float value);



	//========================================
	//			���͂̃r�b�g���擾
	//========================================
	uint64_t state();
	uint64_t state(const uint64_t& keyLabel);
	uint64_t trigger();
	uint64_t trigger(const uint64_t& keyLabel);
	uint64_t release();
	uint64_t release(const uint64_t& keyLabel);


	//========================================
	//		�J�[�\���̍��W���擾
	//========================================
	DirectX::XMFLOAT2 getCursorPos();

	int getCursorPosX();

	int getCursorPosY();

	const bool cursorIsMoved();


	// --- �J�[�\���̕ω��ʂ̎擾 ---
	int getCursorDeltaX();

	int getCursorDeltaY();

	// --- �}�E�X�z�C�[���̉�]�̎擾 ---
	int getMouseWheel();
}


// ===== �I�[�f�B�I�֘A ==================================================================================================================================
class AudioManager;

namespace audio
{
	// --- �C���X�^���X�̎擾 ---
	AudioManager& get();

	// --- �S�̂̉��ʂ̎擾 ---
	float getVolume();

	// --- �S�̂̉��ʂ̐ݒ� ---
	void setVolume(float volume);
}


// ===== ���y�֘A =======================================================================================================================================
namespace music
{
	// --- �ŏI�I�ȉ��ʂ̎擾 ---
	// - �߂�l : ���� �~ ���y�S�̂̉��� �~ �S�̂̉��� �̌���
	float getFinalVolume(int index);


	// --- �S�̂̉��ʂ̎擾 ---
	float getMasterVolume();

	// --- �S�̂̉��ʂ̐ݒ� ---
	void setMasterVolume(float volume);


	// --- ���ʂ̎擾 ---
	float getVolume(int index);

	// --- ���ʂ̐ݒ� ---
	void setVolume(int index, float volume);


	// --- �ǂݍ��� ---
	void load(const char* fileName, float volume = 1.0f);

	// --- �Đ� ---
	void play(int index);

	// --- ��~ ---
	void stop(int index);

	// --- �ꊇ��~ ---
	void stop();
}


// ===== ���ʉ��֘A =======================================================================================================================================
namespace sound
{
	// --- �ŏI�I�ȉ��ʂ̎擾 ---
	// - �߂�l : ���� �~ ���y�S�̂̉��� �~ �S�̂̉��� �̌���
	float getFinalVolume(int index);


	// --- �S�̂̉��ʂ̎擾 ---
	float getMasterVolume();

	// --- �S�̂̉��ʂ̐ݒ� ---
	void setMasterVolume(float volume);


	// --- ���ʂ̎擾 ---
	float getVolume(int index);

	// --- ���ʂ̐ݒ� ---
	void setVolume(int index, float volume);


	// --- �ǂݍ��� ---
	void load(const char* fileName, float volume = 1.0f);

	// --- �Đ� ---
	void play(int index);

	// --- ��~ ---
	void stop(int index);

	// --- �ꊇ��~ ---
	void stop();
}


//====================================================================================================
//			���w�֘A
//====================================================================================================
namespace math {

	//=============================================
	//			�l�̐����Ɣ͈͊O�̊m�F
	//=============================================
	
	//--------------------------------
	//		�l�̐���
	//--------------------------------
	float clamp(const float& v, const float& min, const float& max);

	//--------------------------------
	//		> �� max  < �� min
	//--------------------------------
	float clamp(const float& v1, const char* mark, const float& v2);


	//------------------------------
	//		�͈͊O�̊m�F
	//------------------------------
	bool isOutOfRange(const float& v, const float& min, const float& max);

	//--------------------------------
	//		> �� max  < �� min
	//--------------------------------
	float isOutOfRange(const float& v1, const char* mark, const float& v2);



	//========================================
	//			�����_���Ȓl�𐶐�
	//========================================
	int rand(const int& range, const int& offset = (int(0)));

	float frand(const float& max = (float(1)), const float& min = (float(0)));


	//===============================================================================================
	//			�����֘A
	//===============================================================================================

	//�l�̌ܓ�
	//float num	   :  �l
	//int    decimalPlace   : �l�̌ܓ�����ʒu
	int round(float num, int decimalPlace);

	//�����؂�グ
	//float num : �l
	int roundUp(float num);



	//===================================
	//			�x�N�g���֘A
	//===================================
	void normalize(DirectX::XMFLOAT2& v);

	// --- �x�N�g���̌v�Z ---
	DirectX::XMFLOAT2 calcVector(
		const DirectX::XMFLOAT2& src,	// ��
		const DirectX::XMFLOAT2& dst,	// �ڕW
		bool isNormalize = true			// ���K���̗L��
	);
}


DirectX::XMFLOAT2 operator+(const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2);
DirectX::XMFLOAT2 operator+=(DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2);

DirectX::XMFLOAT2 operator-(const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2);
DirectX::XMFLOAT2 operator-=(DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2);

DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2);
DirectX::XMFLOAT2 operator*=(DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2);
DirectX::XMFLOAT2 operator*(const DirectX::XMFLOAT2& v1, const float& v2);

DirectX::XMFLOAT2 operator/(const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2);
DirectX::XMFLOAT2 operator/=(DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2);



//====================================================================================================
//			�C�[�W���O�֘A
//====================================================================================================
namespace easing {

	struct Data {

		float incrementingTimer_ = 0;
		float incrementingTimerLimit_ = 0;
		float decrementingTimer_ = 0;
		float decrementingTimerLimit_ = 0;
		bool  isReverse_ = false;
		EasingFunction function_ = nullptr;

		Data() {};

		Data(float incrementingTimerLimit, float decrementingTimerLimit, bool isReverse, EasingFunction function) :
			incrementingTimerLimit_(incrementingTimerLimit),
			incrementingTimer_(0),
			decrementingTimerLimit_(decrementingTimerLimit),
			decrementingTimer_(decrementingTimerLimit),
			isReverse_(isReverse),
			function_(function)
		{}

		void clear() {

			incrementingTimer_ = 0;
			decrementingTimer_ = decrementingTimerLimit_;
		}
	};

	EasingFunction getFunction(EasingFunctions function);

	bool calculate(Data& data, float& target, float increaseAmount, float elapsedTime);
	bool calculate(std::shared_ptr<Data> data, float& target, float increaseAmount, float elapsedTime);
}



//====================================================================================================
//			�[�x�X�e���V���֘A
//====================================================================================================
namespace depthStencil {

	void set(const int& state);


	enum STATE
	{
		NONE,			//	�ʏ�
		MASK,			//	�}�X�N����
		APPLY_MASK,		//	�}�X�N�ɕ`��
		EXCLUSIVE,		//	�}�X�N�ȊO�ɕ`��
	};
}



//====================================================================================================
//			�T���v���[�֘A
//====================================================================================================
namespace sampler {

	void set();
}



//====================================================================================================
//			�u�����h�֘A
//====================================================================================================
namespace blend {

	void set(int state);

	enum STATE {

		NONE,
		ALPHA,
		ADD,
	};
}



//====================================================================================================
//			���X�^���C�Y�֘A
//====================================================================================================
namespace rasterize {

	void set(int state);

	enum STATE {

		NONE,
		CULL_BACK,
		WIRE_FLAME,
	};
}



//====================================================================================================
//			�����_�[�^�[�Q�b�g�֘A
//====================================================================================================
namespace renderTarget {

	void set();
}