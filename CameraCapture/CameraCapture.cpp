// CameraCapture.cpp : �������̨Ӧ�ó������ڵ㡣
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

