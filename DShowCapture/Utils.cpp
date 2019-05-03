#include "stdafx.h"
#include "Utils.h"
#include <errors.h>

#define MAX_ERROR_TEXT_LEN 1024
BOOL Utils::hrcheck(HRESULT hr, TCHAR* errtext)
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


