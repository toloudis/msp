#include "stdafx.h"
#include "LocalizedStrings.h"
#include "resource.h"

BSTR LoadLocalizedString(UINT stringID)
{
    CString retString;
    retString.LoadString(stringID);
    return retString.AllocSysString();
}