// CameraCapture.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <DeviceManager.h>

int main()
{
	CoInitialize(NULL);
	DeviceList deviceList;
	DeviceManager::instance().listVideoDevice(deviceList);
    return 0;
}

