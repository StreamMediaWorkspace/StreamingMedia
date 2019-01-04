#pragma once
#include <string>
#include <vector>
#include <atlcomcli.h>
#include <strmif.h>
#include <uuids.h>
#include <comutil.h>
#include <guiddef.h>

using namespace std;

typedef struct DeviceInfo {
	std::string displayName;
	std::string deviceId;
	std::string friendName;
	CComPtr<IBaseFilter> pFilter;
}DeviceInfo;

typedef std::vector<DeviceInfo> DeviceList;

class DeviceManager
{
public:
	DeviceManager();
	~DeviceManager();

	static DeviceManager& instance();

	bool listVideoDevice(DeviceList&  filterList);

private:
	bool listDevice(const IID & inCategory, DeviceList&  filterList);
};

