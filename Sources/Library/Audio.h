#pragma once

#include <xaudio2.h>
#include <vector>
#include <memory>
#include <wrl.h>

// ===== オーディオクラス ==================================================================================================================================
class Audio
{
public:
	Audio(IXAudio2* xAudio2, const char* fileName, float volume, float masterVolumeRate);	// コンストラクタ
	~Audio();	// デストラクタ

	float getFinalVolume();					// 最終的な音量の取得
	float getVolume() { return volume_; }	// 音量の取得

	void setVolume(float volume, float masterVolumeRate);	// 音量の設定

	void load(IXAudio2* xAudio2, const char* fileName);						// 読み込み
	void play(int loopCount = 0);											// 再生
	void stop(bool playTails = true, bool waitForBufferToUnqueue = true);	// 停止
	bool isQueued();														// 一時停止

private:
	WAVEFORMATEXTENSIBLE	wfx{ 0 };
	XAUDIO2_BUFFER			buffer_{ 0 };
	IXAudio2SourceVoice*	sourceVoice_;

	float volume_;	// 音量
};


// ===== オーディオ管理クラス =============================================================================================================================
class AudioManager
{
private:
	AudioManager();		// コンストラクタ
	~AudioManager();		// デストラクタ

	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;
	AudioManager(AudioManager&&) noexcept = delete;
	AudioManager& operator= (AudioManager&&) noexcept = delete;

public:
	// --- インスタンスを取得 ---
	static AudioManager& instance()
	{
		static AudioManager instance;
		return instance;
	}

	// --- 音量関連 ---
	float getMasterVolume() { return masterVolume_; }							// オーディオ全体の音量の取得
	float getMusicMasterVolume() { return musicMasterVolume_; }					// 音楽全体の音量の取得
	float getSoundMasterVolume() { return soundMasterVolume_; }					// 効果音全体の音量の取得
	void  setMasterVolume(float volume) { masterVolume_ = volume; }				// オーディオ全体の音量の設定
	void  setMusicMasterVolume(float volume) { musicMasterVolume_ = volume; }	// 音楽全体の音量の設定
	void  setSoundMasterVolume(float volume) { soundMasterVolume_ = volume; }	// 効果音全体の音量の設定

	// --- 音楽関連 ---
	float getMusicVolume(int index) { return soundTrack_.at(index)->getVolume(); }				// 音量の取得
	float getMusicFinalVolume(int index) { return soundTrack_.at(index)->getFinalVolume(); }	// 最終的な音量の取得									
	void  setMusicVolume(int index, float volume) { soundTrack_.at(index)->setVolume(volume, masterVolume_ * musicMasterVolume_); }	// 音量の設定

	void  loadMusic(const char* fileName, float volume = 1.0f);	// 読み込み
	void  playMusic(int index, bool isLoop = true);				// 再生
	void  stopMusic(int index);									// 停止
	void  stopMusic();											// 一括停止

	// --- 効果音関連 ---
	float getSoundVolume(int index) { return soundEffects_.at(index)->getVolume(); }			// 音量の取得
	float getSoundFinalVolume(int index) { return soundEffects_.at(index)->getFinalVolume(); }	// 最終的な音量の取得									
	void  setSoundVolume(int index, float volume) { soundEffects_.at(index)->setVolume(volume, masterVolume_ * soundMasterVolume_); }	// 音量の設定

	void  loadSound(const char* fileName, float volume = 1.0f);	// 読み込み
	void  playSound(int index);									// 再生
	void  stopSound(int index);									// 停止
	void  stopSound();											// 一括停止


private:
	inline static constexpr int MAxCONCURRENT_SOUNDS = 8;	// 効果音の最大同時再生数

	// --- 効果音クラス ---
	class Sound
	{ 
	public:
		Sound(IXAudio2* xAudio2, const char* fileName, float volume, float masterVolumeRate);	// コンストラクタ
		~Sound() { for (auto& sound : sounds_) sound = nullptr; }		// デストラクタ

		float getVolume();		// 音量の取得
		float getFinalVolume();	// 最終的な音量の取得

		void setVolume(float volume, float masterVolumeRate);	// 音量の設定

		bool play();	// 再生
		void stop();	// 停止

	private:
		std::unique_ptr<Audio> sounds_[MAxCONCURRENT_SOUNDS];
	};


	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;			// XAudio2
	IXAudio2MasteringVoice* masterVoice_ = nullptr;		// マスターボイス

	std::vector<std::unique_ptr<Audio>> soundTrack_;	// 音楽
	std::vector<std::unique_ptr<Sound>> soundEffects_;	// 効果音

	float masterVolume_ = 1.0f;			// 全体の音量
	float musicMasterVolume_ = 1.0f;	// 音楽全体の音量
	float soundMasterVolume_ = 1.0f;	// 効果音全体の音量
};