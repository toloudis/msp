// SkpStudioGpuExport.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "SkpStudioGpuExport.h"

//Support plug-in presence in memory for installer
class AutoMutex
{
public:
	AutoMutex()
	{
		m_mutex = CreateMutex( 0, 0, TEXT("{0FED30FF-9F2A-4210-BF82-5EA4BEA45207}") );
	}

	~AutoMutex()
	{
		CloseHandle( m_mutex );
	}

private:
	HANDLE m_mutex;
};


AutoMutex g_mutex;
	
//---------------------------

class CSkpStudioGpuExportModule : public CAtlDllModuleT< CSkpStudioGpuExportModule >
{
public :
	DECLARE_LIBID(LIBID_SkpStudioGpuExportLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SKPSTUDIOGPUEXPORT, "{23E4FB87-3184-4F3E-BB4A-95271C9B952F}")
};

CSkpStudioGpuExportModule _AtlModule;


#ifdef _MANAGED
#pragma managed(push, off)
#endif

// DLL Entry Point
/*
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
    return _AtlModule.DllMain(dwReason, lpReserved); 
}
*/

#ifdef _MANAGED
#pragma managed(pop)
#endif




// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

