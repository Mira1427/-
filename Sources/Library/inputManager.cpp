#include "inputManager.h"
#include "fileManager.h"

#include <Windows.h>


//	�L�[�{�[�h�̊��蓖��
std::list<InputAssign> keyAssign = {
	{"Left", input::LEFT, DirectX::Keyboard::A},
	{"Right", input::RIGHT, DirectX::Keyboard::D},
	{"Up", input::UP, DirectX::Keyboard::W},
	{"Down", input::DOWN, DirectX::Keyboard::S},
	{"Delete", input::DELETE_, DirectX::Keyboard::Delete},
	{"---keyAssignEnd---", -1, -1}
};


//	�Q�[���p�b�h�̊��蓖��
std::list<InputAssign> padAssign = {
	{"---PadAssignEnd---", -1, -1}
};


//	�}�E�X�̊��蓖��
std::list<InputAssign> mouseAssign = {
	{"LMB", input::LMB, VK_LBUTTON},
	{"RMB", input::RMB, VK_RBUTTON},
	{"MMB", input::MMB, VK_MBUTTON},
	{"---MouseAssignEnd---", -1, -1}
};


InputManager::~InputManager() {
	
	//	�I�����ɕۑ�
	saveAssignment();

	k_.assign_.clear();
	p_.padAssign_.clear();
	m_.assign_.clear();

	inputBindings_.clear();
}


void InputManager::initialize() {

	keyboard_	= std::make_unique<DirectX::Keyboard>();
	mouse_		= std::make_unique<DirectX::Mouse>();
	gamePad_	= std::make_shared<DirectX::GamePad>();


	//	�L�[���蓖�Ă̓ǂݍ���
	loadAssignment();
}


//	���蓖�Ă̓ǂݍ���
void InputManager::loadAssignment() {

	//	�������񏉊���
	k_.assign_.clear();
	p_.padAssign_.clear();
	m_.assign_.clear();

	//	�ǂݍ���
	FileManager::load(std::make_unique<KeyAssignFile>());

	//	���蓖�Ă��Ȃ���΃f�t�H���g��ݒ�
	if (k_.assign_.empty() || p_.padAssign_.empty() || m_.assign_.empty()) {

		if(k_.assign_.empty()) 
			k_.assign_ = keyAssign;

		if (p_.padAssign_.empty())
			p_.padAssign_ = padAssign;

		if (m_.assign_.empty())
			m_.assign_ = mouseAssign;

		//	�ݒ��ۑ�
		saveAssignment();
	}


	//	���蓖�Ă��o�C���h����
	bindInputAssign();
}


//	���蓖�Ă̕ۑ�
void InputManager::saveAssignment() {

	FileManager::save(std::make_unique<KeyAssignFile>());
}


//	���蓖�Ă�ꂽ �L�[(�{�^��) ���o�C���h����
void InputManager::bindInputAssign() {

	inputBindings_.clear();

	//	�L�[�{�[�h
	if (!k_.assign_.empty()/*���蓖�čς�*/) {

		for (auto& assign : k_.assign_) {

			inputBindings_[assign.key_] |= assign.bit_;
		}
	}


	//	�Q�[���p�b�h
	if (!p_.padAssign_.empty()/*���蓖�čς�*/ && isConnected(0)/*�ڑ��ς�*/) {

		for (auto& assign : p_.padAssign_) {

			inputBindings_[assign.key_] |= assign.bit_;
		}
	}


	//	�}�E�X
	if (!m_.assign_.empty()/*���蓖�čς�*/) {

		for (auto& assign : m_.assign_) {

			inputBindings_[assign.key_] |= assign.bit_;
		}
	}
}



void InputManager::update() {

	//	�ʂ̃L�[���͗p
	keyboardStateTracker_.Update(keyboard_->GetState());


	//	���͂̏�Ԃ̍X�V
	bit_.prevState_ = bit_.state_;


	//	��Ԃ����Z�b�g
	bit_.state_ = 0;
	bit_.trigger_ = 0;
	bit_.release_ = 0;


	//	�e���̓f�o�C�X�̏�Ԃ��擾
	k_.state_ = keyboard_->GetState();
	m_.state_ = mouse_->GetState();
	p_.state_ = gamePad_->GetState(0);


	//	�L�[�{�[�h
	if (!k_.assign_.empty()/*���蓖�čς�*/) {

		for (auto& assign : k_.assign_) {

			//	�I���R�[�h�̊m�F
			if (assign.bit_ < 0) break;

			//	������Ă�����r�b�g�𗧂Ă�
			if (k_.state_.IsKeyDown(static_cast<DirectX::Keyboard::Keys>(assign.code_)))
				bit_.state_ |= inputBindings_[assign.key_];
		}
	}


	//	�Q�[���p�b�h
	//	���X�e�B�b�N
	float LStickX = 0, LStickY = 0;

	LStickX = p_.state_.thumbSticks.leftX;
	LStickY = p_.state_.thumbSticks.leftY;

	//	�L�[�{�[�h�̓��͏���
	switch (bit_.state_ & (input::LEFT | input::RIGHT)) {
	case input::LEFT:	LStickX = -1.0f;	break;
	case input::RIGHT:	LStickX = 1.0f;		break;
	}

	switch (bit_.state_ & (input::UP | input::DOWN)) {
	case input::UP:		LStickY = 1.0f;		break;
	case input::DOWN:	LStickY = -1.0f;	break;
	}

	p_.stick_.leftx = LStickX;
	p_.stick_.leftY_ = LStickY;

	//	�E�X�e�B�b�N
	p_.stick_.rightx = p_.state_.thumbSticks.rightX;
	p_.stick_.rightY_ = p_.state_.thumbSticks.rightY;

	//	�g���K�[
	p_.trigger_.left_ = p_.state_.triggers.left;
	p_.trigger_.right_ = p_.state_.triggers.right;

	if (!p_.padAssign_.empty()/*���蓖�čς�*/ && isConnected(0)/*�ڑ��ς�*/) {

		for (auto& assign : p_.padAssign_) {

			//	�I���R�[�h�̊m�F
			if (assign.bit_ < 0) break;

			struct InputData { bool data[10]; } inputData{};

			//	�X�e�B�b�N
			if (assign.code_ >= input::GamePad::LSTICK_UP) {

				//	������Ă�����r�b�g�𗧂Ă�
				if (p_.stick_.isTilted(assign.code_))
					bit_.state_ |= inputBindings_[assign.key_];
			}

			//	�g���K�[
			else if (assign.code_ >= input::GamePad::L2) {

				//	������Ă�����r�b�g�𗧂Ă�
				if (p_.trigger_.isTriggered(assign.code_))
					bit_.state_ |= inputBindings_[assign.key_];
			}

			//	�{�^��
			else if (assign.code_ >= input::GamePad::A) {

				//	�{�^���̍\���̂̃f�[�^���R�s�[
				std::memcpy(&inputData, &p_.state_.buttons, sizeof(DirectX::GamePad::Buttons));

				//	������Ă�����r�b�g�𗧂Ă�
				if (inputData.data[assign.code_ - input::GamePad::A])
					bit_.state_ |= inputBindings_[assign.key_];
			}

			//	�\���L�[
			else {

				//	�\���L�[�̍\���̂̃f�[�^���R�s�[
				std::memcpy(&inputData, &p_.state_.dpad, sizeof(DirectX::GamePad::DPad));

				//	������Ă�����r�b�g�𗧂Ă�
				if (inputData.data[assign.code_])
					bit_.state_ |= inputBindings_[assign.key_];
			}
		}
	}


	//	�}�E�X
	
	//	�O��̃J�[�\���̍��W��ۑ�
	m_.prevPosx = m_.curPosx;
	m_.prevPosY_ = m_.curPosY_;

	//	�J�[�\���̍��W���擾
	m_.curPosx = m_.state_.x;
	m_.curPosY_ = m_.state_.y;

	m_.deltax = m_.curPosx - m_.prevPosx;
	m_.deltaY_ = m_.curPosY_ - m_.prevPosY_;

	m_.isMove_ = (std::abs(m_.curPosx - m_.prevPosx) > m_.deadZone_ && std::abs(m_.curPosY_ - m_.prevPosY_) > m_.deadZone_) ? true : false;

	m_.assign_ = mouseAssign;

	if (!m_.assign_.empty()/*���蓖�čς�*/) {

		for (auto& assign : m_.assign_) {

			if (GetAsyncKeyState(assign.code_))
				bit_.state_ |= inputBindings_[assign.key_];
		}
	}



	bit_.trigger_ = ~bit_.prevState_ & bit_.state_;
	bit_.release_ = bit_.prevState_ & ~bit_.state_;
}



//	���͂̃r�b�g��Ԃ�
uint64_t InputManager::state() {

	return bit_.state_;
}

uint64_t InputManager::down() {

	return bit_.trigger_;
}

uint64_t InputManager::up() {

	return bit_.release_;
}