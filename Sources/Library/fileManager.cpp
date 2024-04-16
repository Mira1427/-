#include "fileManager.h"
#include "inputManager.h"
#include "library.h"

#include <fstream>


//	書き込み
void FileManager::save(std::unique_ptr<File> fileData) {

	std::ofstream file(fileData->name_);

	if (file.is_open()) {

		fileData->save(file);

		file.close();

		fileData = nullptr;
	}

	else {

		_ASSERT(!"outputでファイルを開けませんでした。");
	}
}


//	読み込み
void FileManager::load(std::unique_ptr<File> fileData) {

	std::ifstream file(fileData->name_);

	if (file.is_open()) {

		fileData->load(file);

		file.close();

		fileData = nullptr;
	}

	else {

		_ASSERT(!"inputでファイルを開けませんでした。");
	}
}



//========================================
//			ファイル操作関連
//========================================
// 
//	割り当ての読み込み
void KeyAssignFile::load(std::ifstream& file) {

	InputAssign assign = {};

	//	キーボード
	while(1) {

		//	データをロード
		file >> assign.key_ >> assign.bit_ >> assign.code_;

		//	初期値なら追加せず終了
		if (assign.bit_ == 0) break;

		//	リストに追加
		InputManager::instance().k_.assign_.emplace_back(assign);

		//	終了フラグかどうかの確認
		if (assign.bit_ < 0) break;
	};


	//	ゲームパッド
	while(1) {

		//	データをロード
		file >> assign.key_ >> assign.bit_ >> assign.code_;

		//	初期値なら追加せず終了
		if (assign.bit_ == 0) break;

		//	リストに追加
		InputManager::instance().p_.padAssign_.emplace_back(assign);

		//	終了フラグかどうかの確認
		if (assign.bit_ < 0) break;
	};


	//	マウス
	while(1) {

		//	データをロード
		file >> assign.key_ >> assign.bit_ >> assign.code_;

		//	初期値なら追加せず終了
		if (assign.bit_ == 0) break;

		//	リストに追加
		InputManager::instance().m_.assign_.emplace_back(assign);

		//	終了フラグかどうかの確認
		if (assign.bit_ < 0) break;
	};
}


//	割り当ての保存
void KeyAssignFile::save(std::ofstream& file) {

	//	キー割り当てを保存
	for (auto& it : InputManager::instance().k_.assign_) {

		file << it.key_ << " " << it.bit_ << " " << it.code_ << '\n';
	}

	//	ゲームパッドの割り当てを保存
	for (auto& it : InputManager::instance().p_.padAssign_) {

		file << it.key_ << " " << it.bit_ << " " << it.code_ << '\n';
	}

	//	マウスの割り当てを保存
	for (auto& it : InputManager::instance().m_.assign_) {

		file << it.key_ << " " << it.bit_ << " " << it.code_ << '\n';
	}
}



//	ウィンドウのサイズとフルスクリーンかの取得
void WindowModeFile::load(std::ifstream& file) {

	float width = window::getWidth();
	float height = window::getHeight();
	bool isFullScreen = window::isFullScreen();
	file >> width >> height >> isFullScreen;
}

void WindowModeFile::save(std::ofstream& file) {

}