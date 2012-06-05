/*****************************************************************************
**  DllEntry.cpp
**
**	The entry/exit point for the plugin dll 
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "Max.h"
#include "resource.h"
#include "SgpuExport.hpp"
#include "SgpuExportFP.hpp"
#include "MaxExportOptions.hpp"

#include "Core/CoreLayer.hpp"
#include "Graphics/GraphicsLayer.hpp"

HINSTANCE hInstance;


#define ERROR_MSG_MAX_LEN   128


TCHAR* GetString(int id)
{
	static TCHAR stBuf[ERROR_MSG_MAX_LEN];
	if (hInstance)
		return LoadString(hInstance, id, stBuf, ERROR_MSG_MAX_LEN) ? stBuf : NULL;
	return NULL;
}

__declspec( dllexport ) const TCHAR *
LibDescription() { return GetString(IDS_LIBDESCRIPTION); }

__declspec( dllexport ) int
LibNumberClasses() { return 3; }

__declspec( dllexport ) ClassDesc *
LibClassDesc(int i) {
	switch(i) {
		case 0:  return &SgpuExportClassDesc::theSgpuExportClassDesc; 
			break;
		case 1: return &SgpuExportFP_GUP::theDesc;
			break;
		case 2: return &SgpuExportOptionsClassDesc::theDesc;
		default: return 0; break;
	}
}

// Return version so can detect obsolete DLLs
__declspec( dllexport ) ULONG   
LibVersion() { return VERSION_3DSMAX; }

// Let the plug-in register itself for deferred loading
__declspec( dllexport ) ULONG CanAutoDefer()
{
	return 1;
}



BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID UNUSED(lpvReserved))
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		hInstance = hinstDLL;
#if MAX_VERSION_MAJOR < 10
		InitCustomControls(hInstance);
#endif // pre-Max 2008 only.
		//What does this mean?
		//DisableThreadLibraryCalls(hInstance);
		break;

	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}