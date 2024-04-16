#pragma once

#include <xaudio2.h>
#include <vector>
#include <memory>
#include <wrl.h>

// ===== �I�[�f�B�I�N���X ==================================================================================================================================
class Audio
{
public:
	Audio(IXAudio2* xAudio2, const char* fileName, float volume, float masterVolumeRate);	// �R���X�g���N�^
	~Audio();	// �f�X�g���N�^

	float getFinalVolume();					// �ŏI�I�ȉ��ʂ̎擾
	float getVolume() { return volume_; }	// ���ʂ̎擾

	void setVolume(float volume, float masterVolumeRate);	// ���ʂ̐ݒ�

	void load(IXAudio2* xAudio2, const char* fileName);						// �ǂݍ���
	void play(int loopCount = 0);											// �Đ�
	void stop(bool playTails = true, bool waitForBufferToUnqueue = true);	// ��~
	bool isQueued();														// �ꎞ��~

private:
	WAVEFORMATEXTENSIBLE	wfx{ 0 };
	XAUDIO2_BUFFER			buffer_{ 0 };
	IXAudio2SourceVoice*	sourceVoice_;

	float volume_;	// ����
};


// ===== �I�[�f�B�I�Ǘ��N���X =============================================================================================================================
class AudioManager
{
private:
	AudioManager();		// �R���X�g���N�^
	~AudioManager();		// �f�X�g���N�^

	AudioManager(const AudioManager&) = delete;
	AudioManager& operator=(const AudioManager&) = delete;
	AudioManager(AudioManager&&) noexcept = delete;
	AudioManager& operator= (AudioManager&&) noexcept = delete;

public:
	// --- �C���X�^���X���擾 ---
	static AudioManager& instance()
	{
		static AudioManager instance;
		return instance;
	}

	// --- ���ʊ֘A ---
	float getMasterVolume() { return masterVolume_; }							// �I�[�f�B�I�S�̂̉��ʂ̎擾
	float getMusicMasterVolume() { return musicMasterVolume_; }					// ���y�S�̂̉��ʂ̎擾
	float getSoundMasterVolume() { return soundMasterVolume_; }					// ���ʉ��S�̂̉��ʂ̎擾
	void  setMasterVolume(float volume) { masterVolume_ = volume; }				// �I�[�f�B�I�S�̂̉��ʂ̐ݒ�
	void  setMusicMasterVolume(float volume) { musicMasterVolume_ = volume; }	// ���y�S�̂̉��ʂ̐ݒ�
	void  setSoundMasterVolume(float volume) { soundMasterVolume_ = volume; }	// ���ʉ��S�̂̉��ʂ̐ݒ�

	// --- ���y�֘A ---
	float getMusicVolume(int index) { return soundTrack_.at(index)->getVolume(); }				// ���ʂ̎擾
	float getMusicFinalVolume(int index) { return soundTrack_.at(index)->getFinalVolume(); }	// �ŏI�I�ȉ��ʂ̎擾									
	void  setMusicVolume(int index, float volume) { soundTrack_.at(index)->setVolume(volume, masterVolume_ * musicMasterVolume_); }	// ���ʂ̐ݒ�

	void  loadMusic(const char* fileName, float volume = 1.0f);	// �ǂݍ���
	void  playMusic(int index, bool isLoop = true);				// �Đ�
	void  stopMusic(int index);									// ��~
	void  stopMusic();											// �ꊇ��~

	// --- ���ʉ��֘A ---
	float getSoundVolume(int index) { return soundEffects_.at(index)->getVolume(); }			// ���ʂ̎擾
	float getSoundFinalVolume(int index) { return soundEffects_.at(index)->getFinalVolume(); }	// �ŏI�I�ȉ��ʂ̎擾									
	void  setSoundVolume(int index, float volume) { soundEffects_.at(index)->setVolume(volume, masterVolume_ * soundMasterVolume_); }	// ���ʂ̐ݒ�

	void  loadSound(const char* fileName, float volume = 1.0f);	// �ǂݍ���
	void  playSound(int index);									// �Đ�
	void  stopSound(int index);									// ��~
	void  stopSound();											// �ꊇ��~


private:
	inline static constexpr int MAxCONCURRENT_SOUNDS = 8;	// ���ʉ��̍ő哯���Đ���

	// --- ���ʉ��N���X ---
	class Sound
	{ 
	public:
		Sound(IXAudio2* xAudio2, const char* fileName, float volume, float masterVolumeRate);	// �R���X�g���N�^
		~Sound() { for (auto& sound : sounds_) sound = nullptr; }		// �f�X�g���N�^

		float getVolume();		// ���ʂ̎擾
		float getFinalVolume();	// �ŏI�I�ȉ��ʂ̎擾

		void setVolume(float volume, float masterVolumeRate);	// ���ʂ̐ݒ�

		bool play();	// �Đ�
		void stop();	// ��~

	private:
		std::unique_ptr<Audio> sounds_[MAxCONCURRENT_SOUNDS];
	};


	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;			// XAudio2
	IXAudio2MasteringVoice* masterVoice_ = nullptr;		// �}�X�^�[�{�C�X

	std::vector<std::unique_ptr<Audio>> soundTrack_;	// ���y
	std::vector<std::unique_ptr<Sound>> soundEffects_;	// ���ʉ�

	float masterVolume_ = 1.0f;			// �S�̂̉���
	float musicMasterVolume_ = 1.0f;	// ���y�S�̂̉���
	float soundMasterVolume_ = 1.0f;	// ���ʉ��S�̂̉���
};