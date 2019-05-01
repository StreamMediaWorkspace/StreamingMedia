#pragma once
#include <string>
#include <windows.h>
namespace Utils
{
	namespace GBK32 {
		typedef struct _IMAGE_BUF
		{
			int             width;
			int             height;
			int             Total;
			unsigned char*  buf;
		}IMAGE_BUF;

		//构建BMP位图文件头  
		void ContructBhh(int nWidth, int nHeight, BITMAPFILEHEADER& bhh);

		// 构建BMP文件信息头
		void ConstructBih(int nWidth, int nHeight, BITMAPINFOHEADER& bih);

		bool SaveDIB2Bmp(int fileNum, const char * fileName, const char * filePath, int iWidth, int iHeight, BYTE *pBuffer);
	};

	std::string wideCharToMultiByte(wchar_t* pWCStrKey);//不要忘记使用完char*后delete[]释放内存
	void yuv2ToGBK32(const char *data, int width, int height, char *outGBK32Data);
	bool saveDIB2Bmp(int fileNum, const char * fileName, const char * filePath, int iWidth, int iHeight, BYTE *pBuffer);
};

