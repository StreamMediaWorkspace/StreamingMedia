#include "stdafx.h"
//#include "C:\\DXSDK\\Samples\\C++\\DirectShow\\BaseClasses\\Mtype.h"
#include "DShowHelper.h"

using namespace DShowHelper;

#define CHECK_HR(s) if (FAILED(s)) {return -1;}
void mediaSubtypeToText(const GUID guid, char *outText, int putMaxSize);
int DShowHelper::EnumCaps(CComPtr<IBaseFilter> pSrc, CComPtr<ICaptureGraphBuilder2> pBuild)
{
    CComPtr<IAMStreamConfig> pConfig;
    try
    {
        //�ҵ�IAMStreamConfig�ӿ�
        CHECK_HR(pBuild->FindInterface(&PIN_CATEGORY_STILL, &MEDIATYPE_Video,
            pSrc, IID_IAMStreamConfig, (void**)&pConfig));

        //��ȡnumber��
        int iCnt, iSize;
        CHECK_HR(pConfig->GetNumberOfCapabilities(&iCnt, &iSize));

        //��ȡcps��Ϣ
        BYTE *pSCC = NULL;
        AM_MEDIA_TYPE *pmt;
        pSCC = new BYTE[iSize];
        if (pSCC == NULL) {
            printf("error");
            return -1;
        }
        //����
        for (int i = 0; i < iCnt; i++)
        {
            char sz[64] = { 0 };
            CHECK_HR(pConfig->GetStreamCaps(i, &pmt, pSCC));	//��ȡ��Ϣ
            mediaSubtypeToText(pmt->subtype, sz, sizeof(sz));
            //�γ���Ϣ�ַ��������ڴ�ӡ��ʾ
            printf("Cap index: %d \r\nMinOutputSize: %ld*%ld \r\nMaxOutputSize: %ld*%ld \r\nMinCroppingSize: %ld*%ld\r\nMaxCroppingSize: %ld*%ld\r\n majortype:%ld\r\nsubtype:",
                i,
                ((VIDEO_STREAM_CONFIG_CAPS*)pSCC)->MinOutputSize.cx,
                ((VIDEO_STREAM_CONFIG_CAPS*)pSCC)->MinOutputSize.cy,
                ((VIDEO_STREAM_CONFIG_CAPS*)pSCC)->MaxOutputSize.cx,
                ((VIDEO_STREAM_CONFIG_CAPS*)pSCC)->MaxOutputSize.cy,
                ((VIDEO_STREAM_CONFIG_CAPS*)pSCC)->MinCroppingSize.cx,
                ((VIDEO_STREAM_CONFIG_CAPS*)pSCC)->MinCroppingSize.cy,
                ((VIDEO_STREAM_CONFIG_CAPS*)pSCC)->MaxCroppingSize.cx,
                ((VIDEO_STREAM_CONFIG_CAPS*)pSCC)->MaxCroppingSize.cy,
                pmt->majortype);
            printf("%s\r\n", sz);
           // DeleteMediaType(pmt);	//ɾ��=============
        }

        delete[] pSCC;
    }
    catch (...)
    {
        MessageBox(NULL, _T("ö��Cap��Ϣʧ�ܣ�"), _T(""), MB_OK);
        return -1;
    }

    return 0;
}

void mediaSubtypeToText(const GUID guid, char *outText, int putMaxSize) {
    if (guid == MEDIASUBTYPE_RGB32) {
        strcpy_s(outText, putMaxSize, "MEDIASUBTYPE_RGB32");
    }
    else if (guid == MEDIASUBTYPE_RGB24) {
        strcpy_s(outText, putMaxSize, "MEDIASUBTYPE_RGB24");
    }
    else if (guid == MEDIASUBTYPE_RGB565) {
        strcpy_s(outText, putMaxSize, "MEDIASUBTYPE_RGB565");
    }
    else if (guid == MEDIASUBTYPE_YUY2) {
        strcpy_s(outText, putMaxSize, "MEDIASUBTYPE_YUY2");
    }
    else if (guid == MEDIASUBTYPE_UYVY) {
        strcpy_s(outText, putMaxSize, "MEDIASUBTYPE_UYVY");
    }
    else if (guid == MEDIASUBTYPE_YV12) {
        strcpy_s(outText, putMaxSize, "MEDIASUBTYPE_YV12");
    }
    else if (guid == MEDIASUBTYPE_MJPG) {
        strcpy_s(outText, putMaxSize, "MEDIASUBTYPE_MJPG");
    }
    else if (guid == MEDIASUBTYPE_NV12) {
        strcpy_s(outText, putMaxSize, "MEDIASUBTYPE_NV12");
    }
    else {
        strcpy_s(outText, putMaxSize, "unknown");
    }
}