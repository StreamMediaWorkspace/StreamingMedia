// main.cpp : 定义控制台应用程序的入口点。
//
#include <Qedit.h>
#include <dshow.h>
#include <qedit.h>
#include <Initguid.h>
#include <atlcomcli.h>

#include "stdafx.h"
#include <strmif.h>

#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <uuids.h>
#include <qedit.h>
#include <amvideo.h>
#include <Errors.h>
#include <control.h>
#include <Dshow.h>
#include <thread>
#include "CallbackObject.h"

#include "DShowHelper.h"

#pragma comment(lib,"Strmiids.lib")
#pragma comment(lib,"Quartz.lib")


/*CComPtr<IBaseFilter> CreateFilterByName(const WCHAR* filterName, const GUID& category)
{
    HRESULT hr = S_OK;
    CComPtr<ICreateDevEnum> pSysDevEnum;
    hr = pSysDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum);
    if (hrcheck(hr, L"Can't create System Device Enumerator"))
        return NULL;

    CComPtr<IEnumMoniker> pEnumCat;
    hr = pSysDevEnum->CreateClassEnumerator(category, &pEnumCat, 0);
    if (hr == S_OK)
    {
        CComPtr<IMoniker> pMoniker;
        ULONG cFetched;
        while (pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
        {
            CComPtr<IPropertyBag> pPropBag;
            hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
            if (SUCCEEDED(hr))
            {
                VARIANT varName;
                VariantInit(&varName);
                hr = pPropBag->Read(L"FriendlyName", &varName, 0);
                if (SUCCEEDED(hr))
                {
                    if (wcscmp(filterName, varName.bstrVal) == 0) {
                        CComPtr<IBaseFilter> pFilter;
                        hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter);
                        if (hrcheck(hr, L"Can't bind moniker to filter object"))
                            return NULL;
                        return pFilter;
                    }
                }
                VariantClear(&varName);
            }
            pMoniker.Release();
        }
    }
    return NULL;
}*/

#include "DShowCapture.h"

int main()
{
    CoInitialize(NULL);
    DShowCapture dshow;
    dshow.EnumCaps();
    dshow.start();
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        DispatchMessage(&msg);
    }
    /*CComPtr<IGraphBuilder> graph;
    graph.CoCreateInstance(CLSID_FilterGraph);
    printf("Building graph...\n");
    HRESULT hr = BuildGraph(graph);*/
    /*if (hr == S_OK)
    {
        printf("Running");
        CComQIPtr<IMediaControl, &IID_IMediaControl> mediaControl(graph);
        hr = mediaControl->Run();
        CHECK_HR(hr, L"Can't run the graph");
        CComQIPtr<IMediaEvent, &IID_IMediaEvent> mediaEvent(graph);
        BOOL stop = FALSE;
        MSG msg;
        while (!stop)
        {
            long ev = 0, p1 = 0, p2 = 0;
            Sleep(500);
            printf(".");
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                DispatchMessage(&msg);
            }

            while (mediaEvent->GetEvent(&ev, &p1, &p2, 0) == S_OK) {
                if (ev == EC_COMPLETE || ev == EC_USERABORT) {
                    printf("Done!\n");
                    stop = TRUE;
                } else if (ev == EC_ERRORABORT) {
                    printf("An error abort occurred: HRESULT=%x\n", p1);
                    mediaControl->Stop();
                    stop = TRUE;
                }
                mediaEvent->FreeEventParams(ev, p1, p2);
            }
        }
    }*/
    CoUninitialize();
    system("pause");
    return 0;
}

