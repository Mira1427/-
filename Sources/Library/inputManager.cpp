#include "inputManager.h"
#include "fileManager.h"

#include <Windows.h>


//	キーボードの割り当て
std::list<InputAssign> keyAssign = {
	{"Left", input::LEFT, DirectX::Keyboard::A},
	{"Right", input::RIGHT, DirectX::Keyboard::D},
	{"Up", input::UP, DirectX::Keyboard::W},
	{"Down", input::DOWN, DirectX::Keyboard::S},
	{"Delete", input::DELETE_, DirectX::Keyboard::Delete},
	{"---keyAssignEnd---", -1, -1}
};


//	ゲームパッドの割り当て
std::list<InputAssign> padAssign = {
	{"---PadAssignEnd---", -1, -1}
};


//	マウスの割り当て
std::list<InputAssign> mouseAssign = {
	{"LMB", input::LMB, VK_LBUTTON},
	{"RMB", input::RMB, VK_RBUTTON},
	{"MMB", input::MMB, VK_MBUTTON},
	{"---MouseAssignEnd---", -1, -1}
};


InputManager::~InputManager() {
	
	//	終了時に保存
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


	//	キー割り当ての読み込み
	loadAssignment();
}


//	割り当ての読み込み
void InputManager::loadAssignment() {

	//	いったん初期化
	k_.assign_.clear();
	p_.padAssign_.clear();
	m_.assign_.clear();

	//	読み込み
	FileManager::load(std::make_unique<KeyAssignFile>());

	//	割り当てがなければデフォルトを設定
	if (k_.assign_.empty() || p_.padAssign_.empty() || m_.assign_.empty()) {

		if(k_.assign_.empty()) 
			k_.assign_ = keyAssign;

		if (p_.padAssign_.empty())
			p_.padAssign_ = padAssign;

		if (m_.assign_.empty())
			m_.assign_ = mouseAssign;

		//	設定を保存
		saveAssignment();
	}


	//	割り当てをバインドする
	bindInputAssign();
}


//	割り当ての保存
void InputManager::saveAssignment() {

	FileManager::save(std::make_unique<KeyAssignFile>());
}


//	割り当てられた キー(ボタン) をバインドする
void InputManager::bindInputAssign() {

	inputBindings_.clear();

	//	キーボード
	if (!k_.assign_.empty()/*割り当て済み*/) {

		for (auto& assign : k_.assign_) {

			inputBindings_[assign.key_] |= assign.bit_;
		}
	}


	//	ゲームパッド
	if (!p_.padAssign_.empty()/*割り当て済み*/ && isConnected(0)/*接続済み*/) {

		for (auto& assign : p_.padAssign_) {

			inputBindings_[assign.key_] |= assign.bit_;
		}
	}


	//	マウス
	if (!m_.assign_.empty()/*割り当て済み*/) {

		for (auto& assign : m_.assign_) {

			inputBindings_[assign.key_] |= assign.bit_;
		}
	}
}



void InputManager::update() {

	//	個別のキー入力用
	keyboardStateTracker_.Update(keyboard_->GetState());


	//	入力の状態の更新
	bit_.prevState_ = bit_.state_;


	//	状態をリセット
	bit_.state_ = 0;
	bit_.trigger_ = 0;
	bit_.release_ = 0;


	//	各入力デバイスの状態を取得
	k_.state_ = keyboard_->GetState();
	m_.state_ = mouse_->GetState();
	p_.state_ = gamePad_->GetState(0);


	//	キーボード
	if (!k_.assign_.empty()/*割り当て済み*/) {

		for (auto& assign : k_.assign_) {

			//	終了コードの確認
			if (assign.bit_ < 0) break;

			//	押されていたらビットを立てる
			if (k_.state_.IsKeyDown(static_cast<DirectX::Keyboard::Keys>(assign.code_)))
				bit_.state_ |= inputBindings_[assign.key_];
		}
	}


	//	ゲームパッド
	//	左スティック
	float LStickX = 0, LStickY = 0;

	LStickX = p_.state_.thumbSticks.leftX;
	LStickY = p_.state_.thumbSticks.leftY;

	//	キーボードの入力処理
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

	//	右スティック
	p_.stick_.rightx = p_.state_.thumbSticks.rightX;
	p_.stick_.rightY_ = p_.state_.thumbSticks.rightY;

	//	トリガー
	p_.trigger_.left_ = p_.state_.triggers.left;
	p_.trigger_.right_ = p_.state_.triggers.right;

	if (!p_.padAssign_.empty()/*割り当て済み*/ && isConnected(0)/*接続済み*/) {

		for (auto& assign : p_.padAssign_) {

			//	終了コードの確認
			if (assign.bit_ < 0) break;

			struct InputData { bool data[10]; } inputData{};

			//	スティック
			if (assign.code_ >= input::GamePad::LSTICK_UP) {

				//	押されていたらビットを立てる
				if (p_.stick_.isTilted(assign.code_))
					bit_.state_ |= inputBindings_[assign.key_];
			}

			//	トリガー
			else if (assign.code_ >= input::GamePad::L2) {

				//	押されていたらビットを立てる
				if (p_.trigger_.isTriggered(assign.code_))
					bit_.state_ |= inputBindings_[assign.key_];
			}

			//	ボタン
			else if (assign.code_ >= input::GamePad::A) {

				//	ボタンの構造体のデータをコピー
				std::memcpy(&inputData, &p_.state_.buttons, sizeof(DirectX::GamePad::Buttons));

				//	押されていたらビットを立てる
				if (inputData.data[assign.code_ - input::GamePad::A])
					bit_.state_ |= inputBindings_[assign.key_];
			}

			//	十字キー
			else {

				//	十字キーの構造体のデータをコピー
				std::memcpy(&inputData, &p_.state_.dpad, sizeof(DirectX::GamePad::DPad));

				//	押されていたらビットを立てる
				if (inputData.data[assign.code_])
					bit_.state_ |= inputBindings_[assign.key_];
			}
		}
	}


	//	マウス
	
	//	前回のカーソルの座標を保存
	m_.prevPosx = m_.curPosx;
	m_.prevPosY_ = m_.curPosY_;

	//	カーソルの座標を取得
	m_.curPosx = m_.state_.x;
	m_.curPosY_ = m_.state_.y;

	m_.deltax = m_.curPosx - m_.prevPosx;
	m_.deltaY_ = m_.curPosY_ - m_.prevPosY_;

	m_.isMove_ = (std::abs(m_.curPosx - m_.prevPosx) > m_.deadZone_ && std::abs(m_.curPosY_ - m_.prevPosY_) > m_.deadZone_) ? true : false;

	m_.assign_ = mouseAssign;

	if (!m_.assign_.empty()/*割り当て済み*/) {

		for (auto& assign : m_.assign_) {

			if (GetAsyncKeyState(assign.code_))
				bit_.state_ |= inputBindings_[assign.key_];
		}
	}



	bit_.trigger_ = ~bit_.prevState_ & bit_.state_;
	bit_.release_ = bit_.prevState_ & ~bit_.state_;
}



//	入力のビットを返す
uint64_t InputManager::state() {

	return bit_.state_;
}

uint64_t InputManager::down() {

	return bit_.trigger_;
}

uint64_t InputManager::up() {

	return bit_.release_;
}