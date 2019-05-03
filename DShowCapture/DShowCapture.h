#pragma once
#include "stdafx.h"
#include "CallbackObject.h"
#include <control.h>
#include "DShowHelper.h"
class DShowCapture
{
public:
    DShowCapture();
    ~DShowCapture();

    void start();

    int EnumCaps();

private:
    CComPtr<IBaseFilter> GetFirstCamera();
    HRESULT BuildGraph();

private:
    CComPtr<ICaptureGraphBuilder2> m_pBuilder2;
    CComPtr<IGraphBuilder> m_pGraph;
    CComQIPtr<IMediaControl> m_mediaControl;
    CComPtr<IBaseFilter> m_pSrc;
};

