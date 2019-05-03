#include "stdafx.h"
#include "CallbackObject.h"


CallbackObject::CallbackObject()
{
}


STDMETHODIMP CallbackObject::SampleCB(double SampleTime, IMediaSample *pSample)
{
    if (!pSample) {
        return E_POINTER;
    }
    long sz = pSample->GetActualDataLength();
    BYTE *pBuf = NULL;
    pSample->GetPointer(&pBuf);
    if (sz <= 0 || pBuf == NULL) {
        return E_UNEXPECTED;
    }

    for (int i = 0; i < sz; i += 2) {
        pBuf[i] = 255 - pBuf[i];
    }
    //pSample->Release();
    return S_OK;
}
