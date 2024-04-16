#pragma once

#include "../Graphics/primitive.h"
#include "inputManager.h"
#include "easing.h"
#include "Vector.h"


//====================================================================================================
//			ウィンドウ関連
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

	//	背景のクリア
	void clear(const DirectX::XMFLOAT4& color);

	void close();

	void onSizeChanged(UINT64 width, UINT64 height);

	void stylize(BOOL fullScreen);
}



//====================================================================================================
//			入力関連
//====================================================================================================
namespace input {

	//--------------------------
	//		入力の初期設定
	//--------------------------
	void initialize();

	//--------------------------
	//		入力の更新処理
	//--------------------------
	void update();

	//==================================================
	//			キー入力
	//==================================================
	
	//--------------------------------------------
	//		キーを押している間
	//--------------------------------------------
	DirectX::Keyboard::State keyPressed();

	//--------------------------------------------
	//		キーを押した瞬間
	//--------------------------------------------
	DirectX::Keyboard::State keyDown();

	//--------------------------------------------
	//		キーを離した瞬間
	//--------------------------------------------
	DirectX::Keyboard::State keyUp();


	//========================================
	//			ゲームパッド関連
	//========================================
	DirectX::GamePad::State getState();

	bool isConnected(const int& padNum);


	//	左スティック
	float getLeftX();
	float getLeftX(float deadZoneValue);
	float getLeftY();
	float getLeftY(float deadZoneValue);
	DirectX::XMFLOAT2 getLeft();
	DirectX::XMFLOAT2 getLeft(float deadZoneValue);
	float getLeftStickDeadZone();
	void setLeftStickDeadZone(float value);

	//	右スティック
	float getRightX();
	float getRightX(float deadZoneValue);
	float getRightY();
	float getRightY(float deadZoneValue);
	DirectX::XMFLOAT2 getRight();
	DirectX::XMFLOAT2 getRight(float deadZoneValue);
	float getRightStickDeadZone();
	void setRightStickDeadZone(float value);

	//	左トリガー
	float getLeftTrigger();
	float getLeftTrigger(float deadZoneValue);
	float getLeftTriggerDeadZone();
	void  setLeftTriggerDeadZone(float value);

	//	右トリガー
	float getRightTrigger();
	float getRightTrigger(float deadZoneValue);
	float getRightTriggerDeadZone();
	void  setRightTriggerDeadZone(float value);



	//========================================
	//			入力のビットを取得
	//========================================
	uint64_t state();
	uint64_t state(const uint64_t& keyLabel);
	uint64_t trigger();
	uint64_t trigger(const uint64_t& keyLabel);
	uint64_t release();
	uint64_t release(const uint64_t& keyLabel);


	//========================================
	//		カーソルの座標を取得
	//========================================
	DirectX::XMFLOAT2 getCursorPos();

	int getCursorPosX();

	int getCursorPosY();

	const bool cursorIsMoved();


	// --- カーソルの変化量の取得 ---
	int getCursorDeltaX();

	int getCursorDeltaY();

	// --- マウスホイールの回転の取得 ---
	int getMouseWheel();
}


// ===== オーディオ関連 ==================================================================================================================================
class AudioManager;

namespace audio
{
	// --- インスタンスの取得 ---
	AudioManager& get();

	// --- 全体の音量の取得 ---
	float getVolume();

	// --- 全体の音量の設定 ---
	void setVolume(float volume);
}


// ===== 音楽関連 =======================================================================================================================================
namespace music
{
	// --- 最終的な音量の取得 ---
	// - 戻り値 : 音量 × 音楽全体の音量 × 全体の音量 の結果
	float getFinalVolume(int index);


	// --- 全体の音量の取得 ---
	float getMasterVolume();

	// --- 全体の音量の設定 ---
	void setMasterVolume(float volume);


	// --- 音量の取得 ---
	float getVolume(int index);

	// --- 音量の設定 ---
	void setVolume(int index, float volume);


	// --- 読み込み ---
	void load(const char* fileName, float volume = 1.0f);

	// --- 再生 ---
	void play(int index);

	// --- 停止 ---
	void stop(int index);

	// --- 一括停止 ---
	void stop();
}


// ===== 効果音関連 =======================================================================================================================================
namespace sound
{
	// --- 最終的な音量の取得 ---
	// - 戻り値 : 音量 × 音楽全体の音量 × 全体の音量 の結果
	float getFinalVolume(int index);


	// --- 全体の音量の取得 ---
	float getMasterVolume();

	// --- 全体の音量の設定 ---
	void setMasterVolume(float volume);


	// --- 音量の取得 ---
	float getVolume(int index);

	// --- 音量の設定 ---
	void setVolume(int index, float volume);


	// --- 読み込み ---
	void load(const char* fileName, float volume = 1.0f);

	// --- 再生 ---
	void play(int index);

	// --- 停止 ---
	void stop(int index);

	// --- 一括停止 ---
	void stop();
}


//====================================================================================================
//			数学関連
//====================================================================================================
namespace math {

	//=============================================
	//			値の制限と範囲外の確認
	//=============================================
	
	//--------------------------------
	//		値の制限
	//--------------------------------
	float clamp(const float& v, const float& min, const float& max);

	//--------------------------------
	//		> で max  < で min
	//--------------------------------
	float clamp(const float& v1, const char* mark, const float& v2);


	//------------------------------
	//		範囲外の確認
	//------------------------------
	bool isOutOfRange(const float& v, const float& min, const float& max);

	//--------------------------------
	//		> で max  < で min
	//--------------------------------
	float isOutOfRange(const float& v1, const char* mark, const float& v2);



	//========================================
	//			ランダムな値を生成
	//========================================
	int rand(const int& range, const int& offset = (int(0)));

	float frand(const float& max = (float(1)), const float& min = (float(0)));


	//===============================================================================================
	//			小数関連
	//===============================================================================================

	//四捨五入
	//float num	   :  値
	//int    decimalPlace   : 四捨五入する位置
	int round(float num, int decimalPlace);

	//小数切り上げ
	//float num : 値
	int roundUp(float num);



	//===================================
	//			ベクトル関連
	//===================================
	void normalize(DirectX::XMFLOAT2& v);

	// --- ベクトルの計算 ---
	DirectX::XMFLOAT2 calcVector(
		const DirectX::XMFLOAT2& src,	// 元
		const DirectX::XMFLOAT2& dst,	// 目標
		bool isNormalize = true			// 正規化の有無
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
//			イージング関連
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
//			深度ステンシル関連
//====================================================================================================
namespace depthStencil {

	void set(const int& state);


	enum STATE
	{
		NONE,			//	通常
		MASK,			//	マスク生成
		APPLY_MASK,		//	マスクに描画
		EXCLUSIVE,		//	マスク以外に描画
	};
}



//====================================================================================================
//			サンプラー関連
//====================================================================================================
namespace sampler {

	void set();
}



//====================================================================================================
//			ブレンド関連
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
//			ラスタライズ関連
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
//			レンダーターゲット関連
//====================================================================================================
namespace renderTarget {

	void set();
}