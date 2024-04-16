#include "String.h"

#include <Windows.h>
#include <codecvt>
#include <locale>

// ���C�h�������UTF-8�ɕϊ�����֐�
std::string WideCharToUTF8(const wchar_t* wideStr) {
	int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);
	std::string utf8Str(utf8Length, 0);
	WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, &utf8Str[0], utf8Length, NULL, NULL);
	return utf8Str;
}
