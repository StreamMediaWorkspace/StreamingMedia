#include "stdafx.h"
#include <windows.h>
#include "Utils.h"

std::string Utils::wideCharToMultiByte(wchar_t* pWCStrKey)
{
	std::string ret = "";
	int pSize = WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), NULL, 0, NULL, NULL);
	char* pCStrKey = new char[pSize + 1];
	WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), pCStrKey, pSize, NULL, NULL);
	pCStrKey[pSize] = '\0';
	ret = pCStrKey;
	delete[] pCStrKey;
	return ret;
}
