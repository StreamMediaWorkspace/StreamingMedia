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

		//����BMPλͼ�ļ�ͷ  
		void ContructBhh(int nWidth, int nHeight, BITMAPFILEHEADER& bhh);

		// ����BMP�ļ���Ϣͷ
		void ConstructBih(int nWidth, int nHeight, BITMAPINFOHEADER& bih);

		bool SaveDIB2Bmp(int fileNum, const char * fileName, const char * filePath, int iWidth, int iHeight, BYTE *pBuffer);
	};

	std::string wideCharToMultiByte(wchar_t* pWCStrKey);//��Ҫ����ʹ����char*��delete[]�ͷ��ڴ�
	void yuv2ToGBK32(const char *data, int width, int height, char *outGBK32Data);
	bool saveDIB2Bmp(int fileNum, const char * fileName, const char * filePath, int iWidth, int iHeight, BYTE *pBuffer);
};

