#include "stdafx.h"
#include "PinHelper.h"
#include "Utils.h"

CComPtr<IPin> PinHelper::GetPin(IBaseFilter *pFilter, LPCOLESTR pinname)
{
    CComPtr<IEnumPins> pEnum;
    CComPtr<IPin> pPin;

    HRESULT hr = pFilter->EnumPins(&pEnum);

    if (Utils::hrcheck(hr, L"Can't enumerate pins."))
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
