/*****************************************************************************
**  DllEntry.cpp
**
**	The entry/exit point for the plugin dll 
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "Max.h"
#include "resource2.h"
#include "MaxModelClusterer.hpp"


HINSTANCE hInstance;


#define ERROR_MSG_MAX_LEN   128


MCHAR* GetString(int id)
{
	static TCHAR stBuf[ERROR_MSG_MAX_LEN];
	if (hInstance)
		return LoadString(hInstance, id, stBuf, ERROR_MSG_MAX_LEN) ? stBuf : NULL;
	return NULL;
}

__declspec( dllexport ) const TCHAR *
LibDescription() { return GetString(IDS_LIBDESCRIPTION); }

__declspec( dllexport ) int
LibNumberClasses() { return 1; }

__declspec( dllexport ) ClassDesc *
LibClassDesc(int i) {
	switch(i) {
		case 0:  return &SgpuModelClustererClassDesc::theSgpuModelClustererClassDesc; 
		default: return 0; break;
	}
}

// Return version so can detect obsolete DLLs
__declspec( dllexport ) ULONG   
LibVersion() { return VERSION_3DSMAX; }

// Let the plug-in register itself for deferred loading
__declspec( dllexport ) ULONG CanAutoDefer()
{
	return 0;
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