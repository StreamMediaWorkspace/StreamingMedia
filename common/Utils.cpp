#include "stdafx.h"
#include <windows.h>
#include "Utils.h"

void Utils::GBK32::ContructBhh(int nWidth, int nHeight, BITMAPFILEHEADER& bhh) //add 2010-9-04  
{
	int widthStep = (((nWidth * 24) + 31) & (~31)) / 8; //每行实际占用的大小（每行都被填充到一个4字节边界）  
	bhh.bfType = ((WORD)('M' << 8) | 'B');  //'BM'  
	bhh.bfSize = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + widthStep * nHeight;
	bhh.bfReserved1 = 0;
	bhh.bfReserved2 = 0;
	bhh.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
}

// 构建BMP文件信息头
void Utils::GBK32::ConstructBih(int nWidth, int nHeight, BITMAPINFOHEADER& bih)
{
	int widthStep = (((nWidth * 24) + 31) & (~31)) / 8;
	bih.biSize = 40;       // header size  
	bih.biWidth = nWidth;
	bih.biHeight = nHeight;
	bih.biPlanes = 1;
	bih.biBitCount = 24;     // RGB encoded, 24 bit  
	bih.biCompression = BI_RGB;   // no compression 非压缩  
	bih.biSizeImage = widthStep*nHeight * 3;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;
}

bool Utils::GBK32::SaveDIB2Bmp(int fileNum, const char * fileName, const char * filePath, int iWidth, int iHeight, BYTE *pBuffer) {
	BITMAPINFOHEADER bih;
	ConstructBih(iWidth, iHeight, bih);
	BITMAPFILEHEADER bhh;
	ContructBhh(iWidth, iHeight, bhh);

	CHAR BMPFileName[1024];
	int widthStep = (((iWidth * 24) + 31) & (~31)) / 8; //每行实际占用的大小（每行都被填充到一个4字节边界）  
	int DIBSize = widthStep * iHeight;  //buffer的大小 （字节为单位）  

										//save  
	char path[1024];
	char str[1024];
	char num[10];
	sprintf(num, "%d", fileNum);
	sprintf(str, fileName);
	strcat(str, num);
	strcat(str, ".bmp");  //frame100.bmp  
	sprintf(path, "%s", filePath);
	strcat(path, str); //Path//frame100.bmp  

	strcpy(BMPFileName, path);
	FILE *file;

	if (file = fopen(BMPFileName, "wb"))
	{
		//写入文件
		fwrite((LPSTR)&bhh, sizeof(BITMAPFILEHEADER), 1, file);
		fwrite((LPSTR)&bih, sizeof(BITMAPINFOHEADER), 1, file);
		fwrite(pBuffer, sizeof(char), DIBSize, file);
		fclose(file);
		return true;
	}
	return false;
}

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

//YUY2  是 4:2:2  [Y0 U0 Y1 V0]
void Utils::yuv2ToGBK32(const char *data, int width, int height, char *outGBK32Data) {
	int index = 0;
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			char y0 = data[j*height * 4 + i];
			char u = data[j*height * 4 + i + 1];
			char y1 = data[j*height * 4 + i + 2];
			char v = data[j*height * 4 + i + 3];

			int data = (int)(y0 + 1.772 * (u - 128));//b分量
			outGBK32Data[index] = ((data < 0) ? 0 : (data > 255 ? 255 : data));

			data = (int)(y0 - 0.34414 * (u - 128) - 0.71414 * (v - 128));//g分量
			outGBK32Data[index + 1] = ((data < 0) ? 0 : (data > 255 ? 255 : data));

			data = (int)(y0 + 1.402 * (v - 128));//r分量
			outGBK32Data[index + 2] = ((data < 0) ? 0 : (data > 255 ? 255 : data));
			index += 3;

			data = (int)(y1 + 1.772 * (u - 128));//b分量
			outGBK32Data[index] = ((data < 0) ? 0 : (data > 255 ? 255 : data));

			data = (int)(y1 - 0.34414 * (u - 128) - 0.71414 * (v - 128));//g分量
			outGBK32Data[index + 1] = ((data < 0) ? 0 : (data > 255 ? 255 : data));

			data = (int)(y1 + 1.402 * (v - 128));//r分量
			outGBK32Data[index + 2] = ((data < 0) ? 0 : (data > 255 ? 255 : data));
			index += 3;
		}
	}
}

bool Utils::saveDIB2Bmp(int fileNum, const char * fileName, const char * filePath, int iWidth, int iHeight, BYTE *pBuffer) {
	return GBK32::SaveDIB2Bmp(fileNum, fileName, filePath, iWidth, iHeight, pBuffer);
}