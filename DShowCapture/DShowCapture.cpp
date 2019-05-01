// DShowCapture.cpp : 定义控制台应用程序的入口点。
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
#include "CallbackObject.h"

#pragma comment(lib,"Strmiids.lib")
#pragma comment(lib,"Quartz.lib")

#define MAX_ERROR_TEXT_LEN 1024

#define CHECK_HR(s, text) if (FAILED(s)) {printf("%s\n", text);return -1;} 
#define CHECK_HR_EX(s, text) if (FAILED(s)) {printf("%s\n", text);return NULL;}

// {C1F400A0-3F08-11D3-9F0B-006008039E37}
DEFINE_GUID(CLSID_SampleGrabber,
    0xC1F400A0, 0x3F08, 0x11D3, 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37); //qedit.dll

BOOL hrcheck(HRESULT hr, TCHAR* errtext)

{

    if (hr >= S_OK)

        return FALSE;

    TCHAR szErr[MAX_ERROR_TEXT_LEN];

    DWORD res = AMGetErrorText(hr, szErr, MAX_ERROR_TEXT_LEN);

    if (res)

        printf("Error %x: %s\n%s\n", hr, errtext, szErr);

    else

        printf("Error %x: %s\n", hr, errtext);

    return TRUE;

}

CComPtr<IBaseFilter> CreateFilterByName(const WCHAR* filterName, const GUID& category)

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

}



CComPtr<IPin> GetPin(IBaseFilter *pFilter, LPCOLESTR pinname)

{

    CComPtr<IEnumPins> pEnum;

    CComPtr<IPin> pPin;



    HRESULT hr = pFilter->EnumPins(&pEnum);

    if (hrcheck(hr, L"Can't enumerate pins."))

        return NULL;



    while (pEnum->Next(1, &pPin, 0) == S_OK)

    {

        PIN_INFO pinfo;

        pPin->QueryPinInfo(&pinfo);

        BOOL found = !wcsicmp(pinname, pinfo.achName);

        if (pinfo.pFilter) pinfo.pFilter->Release();

        if (found)

            return pPin;

        pPin.Release();

    }

    printf("Pin not found!\n");

    return NULL;

}

HRESULT GetCamera(CComPtr<IBaseFilter> &src) {
    ICreateDevEnum *pDevEnum = NULL;
    IEnumMoniker *pClsEnum = NULL;
    IMoniker *pMoniker = NULL;
    //创建设备枚举COM对象  
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, (void **)&pDevEnum);
    CHECK_HR(hr, "");
    //创建视频采集设备枚举COM对象  
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClsEnum, 0);
    CHECK_HR(hr, "");

    int i = 0;
    while (i <= 0)
    {
        hr = pClsEnum->Next(1, &pMoniker, NULL);
        ++i;
    }
    CHECK_HR(hr, "");

    //IBaseFilter *m_pSrc;
    hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&src);//就是这句获得Filter  
    CHECK_HR(hr, "");

    return 0;
}

HRESULT BuildGraph(IGraphBuilder *pGraph)
{
    HRESULT hr = S_OK;
    if (!pGraph) {
        hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
            IID_IGraphBuilder, (void **)&pGraph);
        CHECK_HR(hr, "create graph error");
    }

    CComPtr<ICaptureGraphBuilder2> pBuilder;
    hr = pBuilder.CoCreateInstance(CLSID_CaptureGraphBuilder2);
    CHECK_HR(hr, _T("Can't create Capture Graph Builder"));
    hr = pBuilder->SetFiltergraph(pGraph);
    CHECK_HR(hr, "Can't SetFiltergraph");

    //add USB2.0 Camera
    CComPtr<IBaseFilter> pUSB20Camera;// = CreateFilterByName(L"USB2.0 Camera", CLSID_VideoCaptureSources);
    GetCamera(pUSB20Camera);

    hr = pGraph->AddFilter(pUSB20Camera, L"USB2.0 Camera");
    CHECK_HR(hr, "Can't add USB2.0 Camera to graph");

    /*AM_MEDIA_TYPE pmt;
    ZeroMemory(&pmt, sizeof(AM_MEDIA_TYPE));
    pmt.majortype = MEDIATYPE_Video;
    pmt.subtype = MEDIASUBTYPE_YUY2;
    pmt.formattype = FORMAT_VideoInfo;
    pmt.bFixedSizeSamples = TRUE;
    pmt.cbFormat = 88;
    pmt.lSampleSize = 614400;
    pmt.bTemporalCompression = FALSE;
    VIDEOINFOHEADER format;
    ZeroMemory(&format, sizeof(VIDEOINFOHEADER));
    format.bmiHeader.biSize = 40;
    format.bmiHeader.biWidth = 640;
    format.bmiHeader.biHeight = 480;
    format.bmiHeader.biPlanes = 1;
    format.bmiHeader.biBitCount = 16;
    format.bmiHeader.biCompression = 844715353;
    format.bmiHeader.biSizeImage = 614400;
    pmt.pbFormat = (BYTE*)&format;
    CComQIPtr<IAMStreamConfig, &IID_IAMStreamConfig> isc(GetPin(pUSB20Camera, L"Capture"));
    hr = isc->SetFormat(&pmt);
    CHECK_HR(hr, _T("Can't set format"));*/

    //add SampleGrabber
    CComPtr<IBaseFilter> pSampleGrabber;
    hr = pSampleGrabber.CoCreateInstance(CLSID_SampleGrabber);
    CHECK_HR(hr, "Can't create SampleGrabber");
    hr = pGraph->AddFilter(pSampleGrabber, L"SampleGrabber");
    CHECK_HR(hr, "Can't add SampleGrabber to graph");
    CComQIPtr<ISampleGrabber, &IID_ISampleGrabber> pSampleGrabber_isg(pSampleGrabber);

    //here we provide our callback:
    hr = pSampleGrabber_isg->SetCallback(new CallbackObject(), 0);
    CHECK_HR(hr, "Can't set callback");

    //connect USB2.0 Camera and SampleGrabber
    hr = pBuilder->RenderStream(NULL, NULL, pUSB20Camera, NULL, pSampleGrabber);
    CHECK_HR(hr, _T("Can't render stream to SampleGrabber"));

    //render the video in a window
    hr = pBuilder->RenderStream(NULL, NULL, pSampleGrabber, NULL, NULL);
    CHECK_HR(hr, "Can't render stream from SampleGrabber");

    return S_OK;
}

void loop() {
    CoInitialize(NULL);
    IGraphBuilder *pGraph = NULL;
    BuildGraph(pGraph);

}
#include <thread>
int main()
{
    CoInitialize(NULL);

    CComPtr<IGraphBuilder> graph;

    graph.CoCreateInstance(CLSID_FilterGraph);

    printf("Building graph...\n");

    HRESULT hr = BuildGraph(graph);

    if (hr == S_OK)

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
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                DispatchMessage(&msg);
            while (mediaEvent->GetEvent(&ev, &p1, &p2, 0) == S_OK)
            {
                if (ev == EC_COMPLETE || ev == EC_USERABORT)
                {
                    printf("Done!\n");
                    stop = TRUE;
                }
                else if (ev == EC_ERRORABORT)
                {
                    printf("An error abort occurred: HRESULT=%x\n", p1);
                    mediaControl->Stop();
                    stop = TRUE;
                }
                mediaEvent->FreeEventParams(ev, p1, p2);
            }
        }
    }
    CoUninitialize();
    system("pause");
    return 0;
}

