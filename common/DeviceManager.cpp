#include "stdafx.h"
#include "Utils.h"
#include "DeviceManager.h"

#define PROPERTY_FRIENDLY_NAME "FriendlyName"

#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "Strmiids.lib")
#pragma comment(lib, "Quartz.lib")

DeviceManager::DeviceManager()
{
}

DeviceManager& DeviceManager::instance()
{
	static DeviceManager inst;
	return inst;
}

bool DeviceManager::listVideoDevice(DeviceList&  filterList) {
	return listDevice(CLSID_VideoInputDeviceCategory, filterList);
}

bool DeviceManager::listDevice(const IID & inCategory, DeviceList&  filterList)
{
	HRESULT hr = NOERROR;
	CComPtr<ICreateDevEnum> pEnumHardware = NULL;

	hr = pEnumHardware.CoCreateInstance(CLSID_SystemDeviceEnum);
	if (FAILED(hr))
	{
		printf("[FmtDeviceManage] FindDevice, Create SystemDeviceEnum failed=%x\n", hr);
		return FALSE;
	}

	CComPtr<IEnumMoniker> pEnumMoniker = NULL;
	hr = pEnumHardware->CreateClassEnumerator(inCategory, &pEnumMoniker.p, 0);
	if (FAILED(hr) || pEnumMoniker == NULL)
	{
		printf("[FmtDeviceManage] FindDevice, Create CreateClassEnumerator failed=%x\n", hr);
		return false;
	}

	pEnumMoniker->Reset();

	ULONG fetched = 0;
	CComPtr<IMoniker> pMoniker;
	DeviceInfo deviceInfo;
	while (SUCCEEDED(pEnumMoniker->Next(1, &pMoniker, &fetched)) && fetched)
	{
		WCHAR * wzDisplayName = NULL;
		CComPtr<IPropertyBag> propertyBag = NULL;
		CComPtr<IBaseFilter> filter = NULL;
		VARIANT name;

		// Get display name
		hr = pMoniker->GetDisplayName(NULL, NULL, &wzDisplayName);
		if (SUCCEEDED(hr))
		{
			deviceInfo.displayName = Utils::wideCharToMultiByte(wzDisplayName);
			CoTaskMemFree(wzDisplayName);
			deviceInfo.deviceId = deviceInfo.displayName;

			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&propertyBag.p);
		}
		if (SUCCEEDED(hr))
		{
			name.vt = VT_BSTR;
			// Get friendly name
			hr = propertyBag->Read(_T(PROPERTY_FRIENDLY_NAME), &name, NULL);
			if (!SUCCEEDED(hr))
				continue;

			char * szName = _com_util::ConvertBSTRToString(name.bstrVal);
			deviceInfo.friendName = szName;
			delete[] szName;

		}
		if (SUCCEEDED(hr))
		{
			if (deviceInfo.friendName.find("virtual-audio-capturer") == std::string::npos) {
				hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&filter.p);
			}
			else {
				printf("[FmtDeviceManage] ListDevice, Skip virtual audio cable device which may cause crash.\n", hr);
			}
		}
		if (SUCCEEDED(hr))
		{
			deviceInfo.pFilter = filter;
			filterList.push_back(deviceInfo);
		}
		pMoniker.Release();
	}
	return true;
}

DeviceManager::~DeviceManager()
{
}
