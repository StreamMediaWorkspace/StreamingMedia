// CameraCapture.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "DeviceManager.h"
#include "DShowCapture.h"

int main()
{
	CoInitialize(NULL);
	DeviceList deviceList;
	DeviceManager::instance().listVideoDevice(deviceList);

	DShowCapture dshow;
	dshow.EnumCaps();
	dshow.SetCaps(0);
	dshow.start();
	system("pause");
    return 0;
}

