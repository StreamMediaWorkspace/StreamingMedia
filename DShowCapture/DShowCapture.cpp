#include "stdafx.h"
#include "DShowCapture.h"
#include "PinHelper.h"

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
#include "Utils.h"


#define CHECK_HR(s, text) if (FAILED(s)) {printf("%s\n", text);return -1;} 
#define CHECK_HR_EX(s, text) if (FAILED(s)) {printf("%s\n", text);return NULL;}

// {C1F400A0-3F08-11D3-9F0B-006008039E37}
DEFINE_GUID(CLSID_SampleGrabber,
    0xC1F400A0, 0x3F08, 0x11D3, 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37); //qedit.dll


DShowCapture::DShowCapture()
{
    if (0 == BuildGraph()) {
        HRESULT hr = m_pGraph->QueryInterface(&m_mediaControl);
        Utils::hrcheck(hr, L"");
    }
}

void DShowCapture::start() {
    m_mediaControl->Run();
}

int DShowCapture::EnumCaps() {
    return DShowHelper::EnumCaps(m_pSrc, m_pBuilder2);
}

HRESULT DShowCapture::BuildGraph()
{
    HRESULT hr = m_pGraph.CoCreateInstance(CLSID_FilterGraph);
    CHECK_HR(hr, "create filter graph error");
    printf("Building graph...\n");

    if (!m_pGraph) {
        hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
            IID_IGraphBuilder, (void **)&m_pGraph);
        CHECK_HR(hr, "create graph error");
    }

    
    hr = m_pBuilder2.CoCreateInstance(CLSID_CaptureGraphBuilder2);
    CHECK_HR(hr, _T("Can't create Capture Graph Builder"));
    hr = m_pBuilder2->SetFiltergraph(m_pGraph);
    CHECK_HR(hr, "Can't SetFiltergraph");

    //add USB2.0 Camera
    //CComPtr<IBaseFilter> pUSB20Camera = CreateFilterByName(L"USB2.0 Camera", CLSID_VideoCaptureSources);
    m_pSrc = GetFirstCamera();

    hr = m_pGraph->AddFilter(m_pSrc, L"USB2.0 Camera");
    CHECK_HR(hr, "Can't add USB2.0 Camera to graph");

    AM_MEDIA_TYPE pmt;
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
    CComQIPtr<IAMStreamConfig, &IID_IAMStreamConfig> isc(PinHelper::GetPin(m_pSrc, L"捕获"));
    hr = isc->SetFormat(&pmt);
    CHECK_HR(hr, _T("Can't set format"));

    //add SampleGrabber
    CComPtr<IBaseFilter> pSampleGrabber;
    hr = pSampleGrabber.CoCreateInstance(CLSID_SampleGrabber);
    CHECK_HR(hr, "Can't create SampleGrabber");
    hr = m_pGraph->AddFilter(pSampleGrabber, L"SampleGrabber");
    CHECK_HR(hr, "Can't add SampleGrabber to graph");
    CComQIPtr<ISampleGrabber, &IID_ISampleGrabber> pSampleGrabber_isg(pSampleGrabber);

    //pSampleGrabber_isg->SetMediaType(&pmt);


    //here we provide our callback:
    hr = pSampleGrabber_isg->SetCallback(new CallbackObject(), 0);
    CHECK_HR(hr, "Can't set callback");

    //connect USB2.0 Camera and SampleGrabber
    hr = m_pBuilder2->RenderStream(NULL, NULL, m_pSrc, NULL, pSampleGrabber);
    CHECK_HR(hr, _T("Can't render stream to SampleGrabber"));

    //render the video in a window
    hr = m_pBuilder2->RenderStream(NULL, NULL, pSampleGrabber, NULL, NULL);
    CHECK_HR(hr, "Can't render stream from SampleGrabber");

    return S_OK;
}

CComPtr<IBaseFilter> DShowCapture::GetFirstCamera() {
    CComPtr<IBaseFilter> src = nullptr;
    ICreateDevEnum *pDevEnum = NULL;
    IEnumMoniker *pClsEnum = NULL;
    IMoniker *pMoniker = NULL;
    //创建设备枚举COM对象  
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, (void **)&pDevEnum);
    CHECK_HR_EX(hr, "");
    //创建视频采集设备枚举COM对象  
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClsEnum, 0);
    CHECK_HR_EX(hr, "");

    int i = 0;
    while (i <= 0)
    {
        hr = pClsEnum->Next(1, &pMoniker, NULL);
        CHECK_HR_EX(hr, "");
        break;
        ++i;
    }


    //IBaseFilter *m_pSrc;
    hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&src);//就是这句获得Filter  
    CHECK_HR_EX(hr, "");

    return src;
}


DShowCapture::~DShowCapture()
{
}
