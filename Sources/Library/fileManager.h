#pragma once
#include <fstream>
#include <memory>

//	抽象クラス
class File {
public:
	wchar_t* name_;

public:
	virtual void load(std::ifstream&) = 0;
	virtual void save(std::ofstream&) = 0;
};


//	派生クラス
class KeyAssignFile final : public File {
public:
	KeyAssignFile() { name_ = L".\\Data\\Saves\\inputAssign.txt"; }
	void load(std::ifstream& file) override;
	void save(std::ofstream& file) override;
};


class WindowModeFile final : public File {
public:
	WindowModeFile() { name_ = L".\\Data\\Saves\\windowData.txt"; }
	void load(std::ifstream& file) override;
	void save(std::ofstream& file) override;
};



class FileManager {
public:
	static FileManager& instance() {

		static FileManager instance_;
		return instance_;
	}

	static void save(std::unique_ptr<File> fileData);
	static void load(std::unique_ptr<File> fileData);
};

