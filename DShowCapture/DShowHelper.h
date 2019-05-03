#pragma once
#include <strmif.h>  
#include <atlbase.h>
#include <uuids.h>

namespace DShowHelper
{
    int EnumCaps(CComPtr<IBaseFilter> pSrc, CComPtr<ICaptureGraphBuilder2> pBuild);
};

