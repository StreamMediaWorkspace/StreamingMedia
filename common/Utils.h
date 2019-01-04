#pragma once
#include <string>

namespace Utils
{
	//不要忘记使用完char*后delete[]释放内存
	std::string wideCharToMultiByte(wchar_t* pWCStrKey);
};

