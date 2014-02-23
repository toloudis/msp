/****************************************************************************\
**  g2dSystemDX11.cpp
**
**	  g2dSystemDX11.cpp defines the g2d screen PAC for windows.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g2d/g2dSystemDX11.hpp"

#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dFullscreenQuad.hpp"
#include "GraphicsDX11/g2d/g2dResourceCounterDX11.hpp"
#include "GraphicsDX11/g2d/g2dWindowPrimaryDX11.hpp"
#include "GraphicsDX11/g2d/private/g2dFontUtilDX11.hpp"
#include "GraphicsDX11/g2d/private/g2dImageCreateDX11.hpp"
#include "GraphicsDX11/g2d/private/g2dImageSaveDX11.hpp"

#include "Core/env/envSTLHelpers.hpp"
//#include "Core/Env/envThread.hpp"
#include "Core/app/appApplication.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g2d/g2dResetHandler.hpp"
#include "GraphicsDX11/eff/effShaderSDKDX11.hpp"

//this is to suppress the d3dx messages
//#define FXDPF
//#include "D3DX11Effects/d3dx11dbg.h"


//============================================================================
//	anonymous namespace for private data and functions
//============================================================================
namespace
{

typedef HRESULT	 (WINAPI * LPCREATEDXGIFACTORY)(REFIID, void ** );
typedef HRESULT	 (WINAPI * LPD3D11CREATEDEVICE)( IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT32, D3D_FEATURE_LEVEL*, UINT, UINT32, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext** );
static HMODULE							  s_hModDXGI = NULL;
static LPCREATEDXGIFACTORY				  s_DynamicCreateDXGIFactory = NULL;
static HMODULE							  s_hModD3D11 = NULL;
static LPD3D11CREATEDEVICE				  s_DynamicD3D11CreateDevice = NULL;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool EnsureD3D11APIs( void )
{
#ifdef FXDPF
	D3DX_FXDPF_Supressed = true;
#endif

	// If both modules are non-NULL, this function has already been called.  Note
	// that this doesn't guarantee that all ProcAddresses were found.
	if( s_hModD3D11 != NULL && s_hModDXGI != NULL )
		return true;
	
	// This may fail if Direct3D 11 isn't installed
	s_hModD3D11 = LoadLibrary( L"d3d11.dll" );
	if( s_hModD3D11 != NULL )
	{
		s_DynamicD3D11CreateDevice = ( LPD3D11CREATEDEVICE )GetProcAddress( s_hModD3D11, "D3D11CreateDevice" );
	}

	if( !s_DynamicCreateDXGIFactory )
	{
		s_hModDXGI = LoadLibrary( L"dxgi.dll" );
		if( s_hModDXGI )
		{
			s_DynamicCreateDXGIFactory = ( LPCREATEDXGIFACTORY )GetProcAddress( s_hModDXGI, "CreateDXGIFactory1" );
		}

		return ( s_hModDXGI != NULL ) && ( s_hModD3D11 != NULL );
	}

	return ( s_hModD3D11 != NULL );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void enum_adapters()
{
	UINT i = 0; 
	IDXGIAdapter * pAdapter; 
	std::vector <IDXGIAdapter*> vAdapters; 
	// must release each adapter
	while(g2dDX11Global::g_pDXGIFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND) 
	{ 
		UINT j = 0;
		IDXGIOutput * pOutput;
		std::vector<IDXGIOutput*> vOutputs;
		// must release each output.
		while(pAdapter->EnumOutputs(j, &pOutput) != DXGI_ERROR_NOT_FOUND)
		{
			vOutputs.push_back(pOutput);
			++j;
		}
		// do something with outputs or adapter
		// then release
		for (std::vector<IDXGIOutput*>::iterator k = vOutputs.begin(); k != vOutputs.end(); ++k)
		{
			(*k)->Release();
		}


		vAdapters.push_back(pAdapter); 
		++i; 
	} 

	 
	for (std::vector<IDXGIAdapter*>::iterator ia = vAdapters.begin(); ia != vAdapters.end(); ++ia)
	{
		(*ia)->Release();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool DoesBGRASupportExist(ID3D11Device *pDevice)
{
	HRESULT hr = S_OK;

	UINT uFormatSupport = 0;
	bool fDevicePassedFormatTests = false;

	hr = pDevice->CheckFormatSupport(
			 DXGI_FORMAT_B8G8R8A8_UNORM,
			 &uFormatSupport
			 );

	if (SUCCEEDED(hr))
	{
		if ((uFormatSupport & D3D11_FORMAT_SUPPORT_TEXTURE2D)
			&& (uFormatSupport & D3D11_FORMAT_SUPPORT_RENDER_TARGET)
			&& (uFormatSupport & D3D11_FORMAT_SUPPORT_DISPLAY)
			)
		{
			// The device is capable of BGRA textures,
			// rendertargets, and swapchains.  This means we're
			// good.
			fDevicePassedFormatTests = true;
		}
	}

	return fDevicePassedFormatTests;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool CheckHardwareCapability(g2dWindowDX11* i_AppWindow)
{
//	if( FAILED( g2dDX11Global::g_pD3D->CheckDeviceFormat( g2dDX11Global::g_Caps.AdapterOrdinal, g2dDX11Global::g_Caps.DeviceType,
//		g2dDX11Global::g_ScreenFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
//		D3DRTYPE_TEXTURE, i_AppWindow->GetBackBufferFormat() ) ) )
//		return false;

	// No fallback defined by this app, so reject any device that 
	// doesn't support at least shader model 5 and the feature level 11.
	D3D_FEATURE_LEVEL level = g2dDX11Global::g_pDevice->GetFeatureLevel();
	if (level < D3D_FEATURE_LEVEL_11_0)
		return false;
	
	DXGI_ADAPTER_DESC adapterDesc;
	HRESULT hr = g2dDX11Global::g_pAdapter->GetDesc(&adapterDesc);
	if (FAILED(hr))
		return false;
	SIZE_T mem = adapterDesc.DedicatedVideoMemory;
	if (mem < 512*1024*1024)
		return false;

	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool find_has_stencil(int i_BitDepth, const DXGI_FORMAT &i_AdapterFormat, const DXGI_FORMAT& i_BackFormat, int i_Adapter)
{
	HRESULT op_result;
	UINT formatSupport;
	op_result = g2dDX11Global::g_pDevice->CheckFormatSupport(DXGI_FORMAT_D24_UNORM_S8_UINT, &formatSupport);
	if (FAILED(op_result))
		return false;
	if (!(formatSupport & D3D11_FORMAT_SUPPORT_DEPTH_STENCIL))
		return false;

	// How can we verify that the depth format is compatible with the given backbuffer format?

	return true;
}

} // end of namespace


//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dSystemDX11::g2dSystemDX11(int i_Adapter /*= 0*/)
:	m_pFontImpl(NULL),
	m_pImageCreator(NULL),
	m_pImageSaver(NULL)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc( 52 );
#endif
	
	// Declare this process to be high DPI aware, and prevent automatic scaling 
	HINSTANCE hUser32 = LoadLibrary( L"user32.dll" );
	if( hUser32 )
	{
		typedef BOOL ( WINAPI* LPSetProcessDPIAware )( void );
		LPSetProcessDPIAware pSetProcessDPIAware = ( LPSetProcessDPIAware )GetProcAddress( hUser32,
																						   "SetProcessDPIAware" );
		if( pSetProcessDPIAware )
		{
			pSetProcessDPIAware();
		}
		FreeLibrary( hUser32 );
	}

	bool ok = EnsureD3D11APIs();
	if (!ok)
	{
		throw g2dScreenInitX();
	}

//	IDXGIFactory * pFactory = NULL;
//	HRESULT hr = ::CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory) );

	IDXGIFactory1 * pFactory = NULL;
	HRESULT hr = s_DynamicCreateDXGIFactory( __uuidof( IDXGIFactory1 ), (void**)(&pFactory) );
	//HRESULT hr = ::CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&pFactory) );

	g2dDX11Global::g_pDXGIFactory = pFactory;

	if (!SUCCEEDED(hr) || pFactory == NULL)
	{
		throw g2dScreenInitX();
	}
	
#define SGPU_D3D_DRIVER D3D_DRIVER_TYPE_HARDWARE
//#define SGPU_D3D_DRIVER D3D_DRIVER_TYPE_WARP
//#define SGPU_D3D_DRIVER D3D_DRIVER_TYPE_REFERENCE

	// Try to create the device with the chosen settings
	IDXGIAdapter1* pAdapter = NULL;
	hr = S_OK;
	D3D_DRIVER_TYPE ddt = SGPU_D3D_DRIVER;
	if( SGPU_D3D_DRIVER == D3D_DRIVER_TYPE_HARDWARE ) 
	{
		hr = pFactory->EnumAdapters1( i_Adapter, &pAdapter );
		if ( FAILED( hr) ) 
		{
			//throw g2dScreenInitX();
			throw g2dHardwareCapabilityX();
		}
		ddt = D3D_DRIVER_TYPE_UNKNOWN;	
	}
	else if (SGPU_D3D_DRIVER == D3D_DRIVER_TYPE_WARP) 
	{
		ddt = D3D_DRIVER_TYPE_WARP;  
		pAdapter = NULL;
	}
	else if (SGPU_D3D_DRIVER == D3D_DRIVER_TYPE_REFERENCE) 
	{
		ddt = D3D_DRIVER_TYPE_REFERENCE;
		pAdapter = NULL;
	}

	g2dD3D11DevicePtr pDevice = NULL;
	g2dD3D11DeviceContextPtr pDeviceContext = NULL;

	// success means we have a valid adapter and driver request
	if( SUCCEEDED( hr ) )
	{
//		D3D_FEATURE_LEVEL fLevel = D3D_FEATURE_LEVEL_10_1;
		D3D_FEATURE_LEVEL fLevel = D3D_FEATURE_LEVEL_11_0;
		D3D_FEATURE_LEVEL featureLevel;

		if( EnsureD3D11APIs() && s_DynamicD3D11CreateDevice != NULL )
			hr = s_DynamicD3D11CreateDevice( pAdapter, ddt, (HMODULE)0, 
#ifdef _DEBUG
		0,//D3D11_CREATE_DEVICE_DEBUG,
#else
		0,//D3D11_CREATE_DEVICE_SINGLETHREADED,
#endif
			&fLevel,
			1,
			D3D11_SDK_VERSION,
			&pDevice, &featureLevel, &pDeviceContext );
		else
			hr = E_FAIL;
		
		if ( FAILED( hr ) ) 
		{
			throw g2dScreenInitX();
		}
	}

	// success means we have a valid device now
	if( SUCCEEDED( hr ) )
	{
		IDXGIDevice1* pDXGIDev = NULL;
		hr = pDevice->QueryInterface( __uuidof( IDXGIDevice1 ), ( LPVOID* )&pDXGIDev );
		if( SUCCEEDED( hr ) && pDXGIDev )
		{
			if ( pAdapter == NULL ) 
			{
				IDXGIAdapter *pTempAdapter;
				pDXGIDev->GetAdapter( &pTempAdapter );
				hr = ( pTempAdapter->QueryInterface( __uuidof( IDXGIAdapter1 ), (LPVOID*) &pAdapter ) );
				if (FAILED(hr)) 
					throw g2dScreenInitX();
				hr = ( pAdapter->GetParent( __uuidof( IDXGIFactory1 ), (LPVOID*) &pFactory ) );
				if (FAILED(hr)) 
					throw g2dScreenInitX();
				SAFE_RELEASE ( pTempAdapter );
				g2dDX11Global::g_pDXGIFactory = pFactory;
			}
		}
		SAFE_RELEASE( pDXGIDev );
		g2dDX11Global::g_pAdapter = pAdapter;
	}

#ifdef _DEBUG
	//ID3D11InfoQueue* pDebugInfo = NULL;
	//hr = pDevice->QueryInterface(__uuidof(ID3D11InfoQueue), reinterpret_cast<void**>(&pDebugInfo));
	//if (SUCCEEDED(hr))
	//{
	//	pDebugInfo->PushEmptyStorageFilter();
	//	// ?
	//	pDebugInfo->Release();
	//}
#endif

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g2dDX11Global::g_pD2DFactory);
	// Create a shared DirectWrite factory.
	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&g2dDX11Global::g_pDWriteFactory)
			);
	}

	// if we have a good device, set it in the global.
	// else throw exception
	g2dDX11Global::g_pDevice = pDevice;
	g2dDX11Global::g_pDeviceContext = pDeviceContext;

// get the current screen format.
	// can i get an accurate value by enumerating outputs?
	///////////////////////////////////////////////////////////
	g2dDX11Global::g_ScreenFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
//	g2dDX11Global::g_ScreenFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	g2dDX11Global::PFDFromD3DFormat(g2dDX11Global::g_ScreenFormat, g2dDX11Global::g_ScreenPixelFormat);
	///////////////////////////////////////////////////////////

	m_pResourceCounterImpl = new g2dResourceCounterDX11();
	g2dResourceCounter::SetImplementation(m_pResourceCounterImpl);

	m_pShaderSDK = new effShaderSDKDX11();
	effShaderSDK::SetImplementation(m_pShaderSDK);

	m_pFontImpl = new g2dFontUtilDX11();
	g2dFontUtil::SetImplementation(m_pFontImpl);

	m_pImageCreator = new g2dImageCreateDX11();
	g2dImageCreate::SetImplementation(m_pImageCreator);

	m_pImageSaver = new g2dImageSaveDX11();
	g2dImageSave::SetImplementation(m_pImageSaver);

	g2dFullscreenQuad::InitFullscreenQuad();

	// we do not need to create an appwindow just for d3d's sake.
	//g2dWindow* w = CreateAppWindow(i_hWnd);
	//g2dWindowDX11* w11 = dynamic_cast<g2dWindowDX11*>(w);

	// make sure we have a device by now!!
	DBG_ASSERT(g2dDX11Global::g_pDevice, "Device not ready to check caps");
	if (!CheckHardwareCapability(NULL))
	{
		DBG_ASSERT(false, "Hardware failed capability check");
		throw g2dScreenInitX();//g2dHardwareCapabilityX();
	}
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dSystemDX11::~g2dSystemDX11()
{
	// Delete windows
	envSTLHelpers::DeleteContainer(m_Windows);

//  Release font neet matTextureMgr Impl, do it in g3dSystem
//	g2dFontUtil::ReleaseAllFonts();

	g2dFullscreenQuad::CleanUpFullscreenQuad();

	// Delete implementations
	delete m_pResourceCounterImpl;
	delete m_pShaderSDK;
	delete m_pFontImpl;
	delete m_pImageCreator;
	delete m_pImageSaver;

	g2dResetHandler::DestroyResetHandlers();

	// Free stuff
	//

	// release the global d3d command flush event query object
	if ( g2dDX11Global::g_pSynchronizationEvent != NULL )
	{
		g2dDX11Global::g_pSynchronizationEvent->Release();
		g2dDX11Global::g_pSynchronizationEvent = NULL;
	}

	if (g2dDX11Global::g_pDWriteFactory != NULL)
	{
		g2dDX11Global::g_pDWriteFactory->Release();
		g2dDX11Global::g_pDWriteFactory = NULL;
	}
	if (g2dDX11Global::g_pD2DFactory != NULL)
	{
		g2dDX11Global::g_pD2DFactory->Release();
		g2dDX11Global::g_pD2DFactory = NULL;
	}

	ULONG RefCount;
	if( g2dDX11Global::g_pDeviceContext )
	{
		// was helpful for debugging resource leaks, but should not be necessary here.
//		g2dDX11Global::g_pDeviceContext->ClearState();
//		g2dDX11Global::g_pDeviceContext->Flush();

		RefCount = g2dDX11Global::g_pDeviceContext->Release();
		if (RefCount > 0L)
		{
			DBG_TRACE("g_pDeviceContext didn't really release, refcount of " << RefCount << ", be sure all g2d, g3d, gui, and fonts have been cleaned up and released");
		}
		g2dDX11Global::g_pDeviceContext = NULL;
	}
	if( g2dDX11Global::g_pDevice )
	{
#ifdef _DEBUG
		//ID3D11Debug* pDebugInfo = NULL;
		//HRESULT hr = g2dDX11Global::g_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&pDebugInfo));
		//if (SUCCEEDED(hr))
		//{
		//	pDebugInfo->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		//	pDebugInfo->Release();//?
		//}
#endif

		RefCount = g2dDX11Global::g_pDevice->Release();
		if (RefCount > 0L)
		{
			DBG_TRACE("g_pDevice didn't really release, refcount of " << RefCount << ", be sure all g2d, g3d, gui, and fonts have been cleaned up and released");
		}
		g2dDX11Global::g_pDevice = NULL;
	}

	if( g2dDX11Global::g_pAdapter )
	{
		RefCount = g2dDX11Global::g_pAdapter->Release();
		if (RefCount > 0L)
		{
			DBG_TRACE("g_pAdapter didn't really release, refcount of " << RefCount << ", be sure all g2d, g3d, gui, and fonts have been cleaned up and released");
		}
		g2dDX11Global::g_pAdapter = NULL;
	}
	if( g2dDX11Global::g_pDXGIFactory )
	{
		RefCount = g2dDX11Global::g_pDXGIFactory->Release();
		if (RefCount > 0L)
		{
			DBG_TRACE("DXGI Factory didn't really release, refcount of " << RefCount << ", be sure all g2d, g3d, gui, and fonts have been cleaned up and released");
		}
		g2dDX11Global::g_pDXGIFactory = NULL;
	}

}

//------------------------------------------------------------------------
//	CreateAppWindow creates a single window	of the given width and
//  height with the upper left corner at i_X and i_Y to be the main
//	window for the application.  If the user specifies invalid
//	coordinates a g2dUnsupportedScreenModeX exception will be thrown.
//------------------------------------------------------------------------
g2dWindow* g2dSystemDX11::CreateAppWindow(int i_Width, int i_Height, int i_X, int i_Y)
{
	appApplication::SetMainWindowSize(i_X, i_Y, i_Width, i_Height);

	g2dWindowDX11* window = new g2dWindowPrimaryDX11((HWND)appApplication::GetMainWindowHandle(), false);
	m_Windows.push_back(window);
	return window;
}


//------------------------------------------------------------------------
//	CreateSubWindow creates a window to be used within a larger
//	application form. Pass in the appropriate OS handle to define
//	the size and location of the window (HWND for MS Windows).
//  If the user specifies invalid coordinates a
//	g2dUnsupportedScreenModeX exception will be thrown.
//------------------------------------------------------------------------
g2dWindow* g2dSystemDX11::CreateAppWindow(void* i_Handle)
{
	g2dWindowDX11* window = new g2dWindowPrimaryDX11((HWND)i_Handle, false);
	m_Windows.push_back(window);
	return window;
}

//------------------------------------------------------------------------
// Creates sub window within given window handle.
//------------------------------------------------------------------------
g2dWindow* g2dSystemDX11::CreateSubWindow(void* i_Handle)
{
	g2dWindowDX11* window = new g2dWindowPrimaryDX11((HWND)i_Handle, false);
	m_Windows.push_back(window);
	return window;
}


//------------------------------------------------------------------------
// Create sub window in new window with given size and position
//------------------------------------------------------------------------
g2dWindow* g2dSystemDX11::CreateSubWindow(int i_Width, int i_Height, int i_X, int i_Y)
{
	itString title("");
	HWND hwnd = (HWND)appApplication::CreateSubWindow(i_Width, i_Height, i_X, i_Y, title);

	g2dWindowDX11* window = new g2dWindowPrimaryDX11(hwnd, true, i_Width, i_Height);
	m_Windows.push_back(window);
	return window;
}

//------------------------------------------------------------------------
//	InitializeFullScreen initializes the drawing system to use the whole
//	screen in the given resolution.  If an unsupported mode is
//	requested, a g2dUnsupportedScreenModeX exception will be thrown.
//------------------------------------------------------------------------
g2dWindow* g2dSystemDX11::CreateFullScreen(int i_Width, int i_Height, int i_BitDepth)
{
	DBG_ERROR("Fullscreen not supported.");
	return NULL;
//	g2dWindowDX11* window = new g2dWindowPrimaryDX11(i_Width, i_Height, i_BitDepth);
//	m_Windows.push_back(window);
//	return window;
}

//------------------------------------------------------------------------
//	InitializeFullScreen initializes a new sub window to use the whole
//	screen in the given resolution.  If an unsupported mode is
//	requested, a g2dUnsupportedScreenModeX exception will be thrown.
//------------------------------------------------------------------------
g2dWindow* g2dSystemDX11::CreateSubFullScreen(int i_Width, int i_Height, int i_BitDepth)
{
	DBG_ERROR("Fullscreen not supported.");
	return NULL;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g2dSystemDX11::DestroyWindow(g2dWindow* i_Window)
{
	envSTLHelpers::DeleteOneValue(m_Windows, i_Window);
}

//------------------------------------------------------------------------
// warning: this call is potentially expensive!
//------------------------------------------------------------------------
void g2dSystemDX11::ClearManagedResources()
{
}

