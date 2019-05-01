#pragma once
#include <qedit.h>
class CallbackObject :
    public ISampleGrabberCB
{
public:
    CallbackObject();

    STDMETHODIMP QueryInterface(REFIID riid, void **ppv)
    {
        if (NULL == ppv) return E_POINTER;
        if (riid == __uuidof(IUnknown)) {
            *ppv = static_cast<IUnknown*>(this);
            return S_OK;
        }
        if (riid == __uuidof(ISampleGrabberCB)) {
            *ppv = static_cast<ISampleGrabberCB*>(this);
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() { return S_OK; }
    STDMETHODIMP_(ULONG) Release() { return S_OK; }

    //ISampleGrabberCB
    STDMETHODIMP SampleCB(double SampleTime, IMediaSample *pSample);
    STDMETHODIMP BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen) { return S_OK; }
};

