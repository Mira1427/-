#include "Audio.h"

#include <Windows.h>
#include <winerror.h>
#include <assert.h>

#include "../misc.h"


// ===== �`�����N�f�[�^�֘A ==============================================================================================================

// --- �`�����N��T�� ---
HRESULT findChunk(HANDLE hFile, DWORD fourCC, DWORD& chunkSize, DWORD& chunkDataPosition)
{
	// --- �t�@�C���|�C���^���t�@�C���̐擪�Ɉړ� ---
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
	{
		// --- �G���[�����������ꍇ�̓G���[�R�[�h��Ԃ� ---
		return HRESULT_FROM_WIN32(GetLastError());
	}

	HRESULT hr{ S_OK };

	// --- �t�@�C���|�C���^�̈ʒu�����i�[����ϐ� ---
	DWORD chunkType;		// �`�����N�̎��
	DWORD chunkDataSize;	// �`�����N�̃f�[�^�T�C�Y
	DWORD riffDataSize = 0;	// RIFF�f�[�^�̃T�C�Y
	DWORD bytesRead = 0;	// �ǂݍ��܂ꂽ�o�C�g��
	DWORD offset = 0;		// �`�����N�̈ʒu

	// --- �t�@�C����ǂݍ��݂Ȃ��珈���𑱂��郋�[�v ---
	while (hr == S_OK)
	{
		DWORD numberOfBytesRead;	// �ǂݍ��񂾃o�C�g��

		// --- �`�����N�̎�ނ�ǂݍ��� ---
		if (0 == ReadFile(hFile, &chunkType, sizeof(DWORD), &numberOfBytesRead, NULL))
		{
			// --- �ǂݎ��G���[�����������ꍇ�̓G���[�R�[�h���Z�b�g ---
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		// --- �`�����N�̃f�[�^�T�C�Y��ǂݎ�� ---
		if (0 == ReadFile(hFile, &chunkDataSize, sizeof(DWORD), &numberOfBytesRead, NULL))
		{
			// --- �ǂݎ��G���[�����������ꍇ�̓G���[�R�[�h���Z�b�g ---
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		// --- �`�����N�̎�ނɉ����ď����𕪊� ---
		switch (chunkType) {
		case 'FFIR': // RIFF�`��
			riffDataSize = chunkDataSize;	// RIFF�f�[�^�̃T�C�Y���擾
			chunkDataSize = 4;				// �`�����N�̃f�[�^�T�C�Y���C��

			// --- �t�@�C���̎�ނ�ǂݎ�� ---
			if (0 == ReadFile(hFile, &chunkType, sizeof(DWORD), &numberOfBytesRead, NULL))
			{
				// --- �ǂݎ��G���[�����������ꍇ�̓G���[�R�[�h���Z�b�g ---
				hr = HRESULT_FROM_WIN32(GetLastError());
			}

			break;

		default: // ����ȊO

			// --- �`�����N�̃f�[�^�T�C�Y���t�@�C���|�C���^���ړ� ---
			if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, chunkDataSize, NULL, FILE_CURRENT))
			{
				// --- �G���[�����������ꍇ�̓G���[�R�[�h��Ԃ� ---
				return HRESULT_FROM_WIN32(GetLastError());
			}

			break;
		}

		offset += sizeof(DWORD) * 2;	// �`�����N�̈ʒu�����X�V

		// --- �ړI��FourCC�ƈ�v����`�����N���������ꍇ ---
		if (chunkType == fourCC)
		{
			// --- �`�����N�̃T�C�Y�ƈʒu�����Z�b�g ---
			chunkSize = chunkDataSize;
			chunkDataPosition = offset;
			return S_OK;
		}

		offset += chunkDataSize;	// ���̃`�����N�̈ʒu�����X�V

		// --- ���t�f�[�^�T�C�Y�ȏ�̃o�C�g����ǂݍ��񂾏ꍇ�͏I�� ---
		if (bytesRead >= riffDataSize)
		{
			return S_FALSE;
		}
	}

	return S_OK;
}

// --- �`�����N��ǂݎ�� ---
HRESULT readChunkData(HANDLE hFile, LPVOID buffer, DWORD bufferSize, DWORD bufferOffset)
{
	HRESULT hr{ S_OK };

	// --- �t�@�C���|�C���^���w�肳�ꂽ�I�t�Z�b�g�ʒu�Ɉړ� ---
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferOffset, NULL, FILE_BEGIN))
	{
		// --- �G���[�����������ꍇ�̓G���[�R�[�h��Ԃ� ---
		return HRESULT_FROM_WIN32(GetLastError());
	}

	DWORD numberOfBytesRead;	// �ǂݎ�����o�C�g��

	// --- �t�@�C������f�[�^��ǂݎ�� ---
	if (0 == ReadFile(hFile, buffer, bufferSize, &numberOfBytesRead, NULL))
	{
		// --- �ǂݎ��G���[�����������ꍇ�̓G���[�R�[�h���Z�b�g ---
		hr = HRESULT_FROM_WIN32(GetLastError());
	}

	return hr;
}


// ===== �I�[�f�B�I�֘A ===================================================================================================================

Audio::Audio(IXAudio2* xAudio2, const char* fileName, float volume, float masterVolumeRate)
{
	load(xAudio2, fileName);				// �ǂݍ���

	setVolume(volume, masterVolumeRate);	// ���ʂ̐ݒ�
}

// --- �f�X�g���N�^ ---
Audio::~Audio()
{
	sourceVoice_->DestroyVoice();	// �\�[�X�{�C�X�����
	delete[] buffer_.pAudioData;	// �I�[�f�B�I�f�[�^�̃o�b�t�@�����
}

// --- �ŏI�I�ȉ��ʂ̎擾 ---
float Audio::getFinalVolume()
{
	float currentVolume;
	sourceVoice_->GetVolume(&currentVolume);
	return currentVolume;
}

// --- ���ʂ̐ݒ� ---
void Audio::setVolume(float volume, float masterVolumeRate)
{
	volume_ = volume;	// ���ʂ�ݒ�

	float finalVolume = volume * masterVolumeRate;

	HRESULT hr{ S_OK };
	hr = sourceVoice_->SetVolume(finalVolume);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

// --- �ǂݍ��� ---
void Audio::load(IXAudio2* xAudio2, const char* fileName)
{
	HRESULT  hr;

	// --- �t�@�C�����J�� ---
	HANDLE hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// --- �t�@�C���|�C���^���t�@�C���̐擪�Ɉړ� ---
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	DWORD chunkSize;		// �`�����N�̃T�C�Y
	DWORD chunkPosition;	// �`�����N�̈ʒu

	// --- �t�@�C���̎�ނ̊m�F ( WAVE��XWMA�̂� ) ---
	findChunk(hFile, 1163280727/*RIFF*/, chunkSize, chunkPosition);
	DWORD fileType;
	readChunkData(hFile, &fileType, sizeof(DWORD), chunkPosition);
	_ASSERT_EXPR(fileType == 'EVAW'/*WAVE*/, L"wave�����Ή����Ă��܂���");

	// --- �t�H�[�}�b�g�`�����N��ǂݍ��� ---
	findChunk(hFile, ' tmf'/*FMT*/, chunkSize, chunkPosition);
	readChunkData(hFile, &wfx, chunkSize, chunkPosition);

	// --- �f�[�^�`�����N����I�[�f�B�I�o�b�t�@�f�[�^���쐬 ---
	findChunk(hFile, 'atad', chunkSize, chunkPosition);
	BYTE* data = new BYTE[chunkSize];
	readChunkData(hFile, data, chunkSize, chunkPosition);

	buffer_.AudioBytes = chunkSize;			// �I�[�f�B�I�o�b�t�@�̃T�C�Y
	buffer_.pAudioData = data;				// �I�[�f�B�I�f�[�^���܂ރo�b�t�@
	buffer_.Flags = XAUDIO2_END_OF_STREAM;	// ���̃o�b�t�@�̌�Ƀf�[�^���Ȃ����Ƃ�`����

	// --- �\�[�X�{�C�X�̍쐬 ---
	hr = xAudio2->CreateSourceVoice(&sourceVoice_, (WAVEFORMATEX*)&wfx);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

// --- �Đ� ---
void Audio::play(int loopCount)
{
	HRESULT hr;

	// --- �\�[�X�{�C�X�̏�Ԃ��擾 ---
	XAUDIO2_VOICE_STATE voiceState = {};
	sourceVoice_->GetState(&voiceState);

	// --- �\�[�X�{�C�X�Ƀo�b�t�@���L���[����Ă���ꍇ�͍Đ����Ȃ� ---
	if (voiceState.BuffersQueued)
		return;

	buffer_.LoopCount = loopCount;	// �o�b�t�@�̃��[�v��

	// --- �\�[�X�{�C�X�Ƀo�b�t�@��񋟂��� ---
	hr = sourceVoice_->SubmitSourceBuffer(&buffer_);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// --- �\�[�X�{�C�X�̍Đ����J�n���� ---
	hr = sourceVoice_->Start(0);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

// --- ��~ ---
void Audio::stop(bool playTails, bool waitForBufferToUnqueue)
{
	// --- �\�[�X�{�C�X�̏�Ԃ��擾 ---
	XAUDIO2_VOICE_STATE voiceState = {};
	sourceVoice_->GetState(&voiceState);

	// --- �\�[�X�{�C�X�Ƀo�b�t�@���L���[����Ă��Ȃ��ꍇ�͍Đ����Ȃ� ---
	if (!voiceState.BuffersQueued)
		return;

	HRESULT hr;

	hr = sourceVoice_->Stop(playTails ? XAUDIO2_PLAY_TAILS : 0);	// �\�[�X�{�C�X���~
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = sourceVoice_->FlushSourceBuffers();	// �\�[�X�{�C�X�̃o�b�t�@���t���b�V��
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// --- �o�b�t�@���L���[����Ă���ԁA�ҋ@����I�v�V�������w�肳��Ă���ꍇ
	while (waitForBufferToUnqueue && voiceState.BuffersQueued)
	{
		sourceVoice_->GetState(&voiceState);
	}

}

// --- �Đ������ǂ��� ---
bool Audio::isQueued()
{
	// --- �\�[�X�{�C�X�̏�Ԃ��擾 ---
	XAUDIO2_VOICE_STATE voiceState = {};
	sourceVoice_->GetState(&voiceState);
	return voiceState.BuffersQueued;
}


// ===== �I�[�f�B�I�Ǘ��N���X =============================================================================================================================

// --- �R���X�g���N�^ ---
AudioManager::AudioManager()
{
	HRESULT hr{ S_OK };

	// --- XAudio2�̍쐬 ---
	hr = XAudio2Create(xAudio2_.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// --- �}�X�^�[�{�C�X�̍쐬 ---
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	soundTrack_.clear();
	soundEffects_.clear();
}

// --- �f�X�g���N�^ ---
AudioManager::~AudioManager()
{
	soundTrack_.clear();
	soundEffects_.clear();
}


// ===== ���y�֘A =======================================================================================================================================

// --- �ǂݍ��� ---
void AudioManager::loadMusic(const char* fileName, float volume)
{
	soundTrack_.emplace_back(std::make_unique<Audio>(xAudio2_.Get(), fileName, volume, masterVolume_ * musicMasterVolume_));
}

// --- �Đ� ---
void AudioManager::playMusic(int index, bool isLoop)
{
	if (index >= soundTrack_.size())
		assert(!"���y�̃C���f�b�N�X���͈͊O�ł�");

	soundTrack_.at(index)->play(isLoop ? 255 : 0);
}

// --- ��~ ---
void AudioManager::stopMusic(int index)
{
	if (index >= soundTrack_.size())
		assert(!"���y�̃C���f�b�N�X���͈͊O�ł�");

	soundTrack_.at(index)->stop();
}

// --- �ꊇ��~ ---
void AudioManager::stopMusic()
{
	for (auto& music : soundTrack_)
	{
		music->stop();
	}
}


// ===== ���ʉ��֘A =======================================================================================================================================

// --- �ǂݍ��� ---
void AudioManager::loadSound(const char* fileName, float volume)
{
	soundEffects_.emplace_back(std::make_unique<Sound>(xAudio2_.Get(), fileName, volume, masterVolume_ * soundMasterVolume_));
}

// --- �Đ� ---
void AudioManager::playSound(int index)
{
	if (index >= soundEffects_.size())
		assert(!"���ʉ��̃C���f�b�N�X���͈͊O�ł�");

	soundEffects_.at(index)->play();
}

// --- ��~ ---
void AudioManager::stopSound(int index)
{
	if (index >= soundEffects_.size())
		assert(!"���ʉ��̃C���f�b�N�X���͈͊O�ł�");

	soundEffects_.at(index)->stop();
}

// --- �ꊇ��~ ---
void AudioManager::stopSound()
{
	for (auto& sound : soundEffects_)
	{
		sound->stop();
	}
}


// ===== ���ʉ��N���X ==================================================================================================================================

// --- �R���X�g���N�^ ---
AudioManager::Sound::Sound(IXAudio2* xAudio2, const char* fileName, float volume, float masterVolumeRate)
{
	for (auto& sound : sounds_)
	{
		sound = std::make_unique<Audio>(xAudio2, fileName, volume, masterVolumeRate);
	}


}

// --- ���ʂ̎擾 ---
float AudioManager::Sound::getVolume()
{
	return sounds_->get()->getVolume();
}

// --- �ŏI�I�ȉ��ʂ̎擾 ---
float AudioManager::Sound::getFinalVolume()
{
	return sounds_->get()->getFinalVolume();
}

// --- ���ʂ̐ݒ� ---
void AudioManager::Sound::setVolume(float volume, float masterVolumeRate)
{
	for (auto& sound : sounds_)
	{
		sound->setVolume(volume, masterVolumeRate);
	}
}

// --- �Đ� ---
bool AudioManager::Sound::play()
{
	for (auto& sound : sounds_) {

		if (!sound->isQueued())	// �Đ�����Ă��Ȃ�������
		{
			sound->play();		// �Đ�
			return true;
		}
	}

	return false;
}

// --- ��~ ---
void AudioManager::Sound::stop()
{
	for (auto& sound : sounds_)
	{
		sound->stop();
	}
}