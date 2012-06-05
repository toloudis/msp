/****************************************************************************\
**  g2dWindowPrimaryDX11.cpp
**
**      g2dWindowPrimaryDX11.hpp supplies the D3D implementation
**	for the primary window for a device.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g2d/g2dWindowPrimaryDX11.hpp"

#include <algorithm>
#include <math.h>

#include "Core/app/appApplication.hpp"
#include "Core/app/appFlowEventHandler.hpp"
#include "Core/app/appCharEvent.hpp"
#include "Core/app/appCharEventHandler.hpp"
#include "Core/app/appTime.hpp"
#include "Core/env/envThread.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g2d/g2dFontUtil.hpp"
#include "Graphics/g2d/g2dResetHandler.hpp"
#include "GraphicsDX11/g2d/g2dDepthStencilBufferDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dStateMgr.hpp"


//----------------------------------------------------------------------------
//	anonymous namespace for private data and functions
//----------------------------------------------------------------------------

namespace
{

// Turn on or off multithreaading in D3D Create Device with this constant:
// Note: this needs to be coordinated with the Mutex in smdlSurface.
// The mutex is needed when D3D is not multithreaded.
//#ifdef ENV_USE_THREADS
//const DWORD c_MULTITHREADING_CREATE_FLAG = D3DCREATE_MULTITHREADED;
//#else
//const DWORD c_MULTITHREADING_CREATE_FLAG = 0;
//#endif

bool l_TripleBuffer = true;

void setup_present_params(	HWND i_Hwnd,
							int i_Width,
							int i_Height,
							DXGI_SWAP_CHAIN_DESC& o_Params,
							const DXGI_FORMAT &i_Format)
{
	::ZeroMemory(&o_Params, sizeof(DXGI_SWAP_CHAIN_DESC));

	// preferred fmt?
	//i_Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	o_Params.BufferDesc.Width = i_Width;
	o_Params.BufferDesc.Height = i_Height;
    o_Params.BufferDesc.RefreshRate.Numerator = 60;
    o_Params.BufferDesc.RefreshRate.Denominator = 1;
	o_Params.BufferDesc.Format = i_Format;
	o_Params.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	o_Params.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	o_Params.SampleDesc.Count = 1;
	o_Params.SampleDesc.Quality = 0;
	o_Params.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	o_Params.BufferCount = 1;//2;//(l_TripleBuffer) ? 3 : 2;
	o_Params.OutputWindow = i_Hwnd;
	o_Params.Windowed = TRUE;
	o_Params.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	o_Params.Flags = 0;
}

} // end of namespace


//------------------------------------------------------------------------
// PLEASE RELEASE AFTER USING
//------------------------------------------------------------------------
g2dD3D11RenderTargetPtr g2dWindowPrimaryDX11::GetBackBuffer()
{
	m_pBackBuffer->AddRef();
	return m_pBackBuffer;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void* g2dWindowPrimaryDX11::GetHandle()
{
	return (void*)(m_Hwnd);
}

//------------------------------------------------------------------------
// General window constructor
//------------------------------------------------------------------------
g2dWindowPrimaryDX11::g2dWindowPrimaryDX11(HWND i_Hwnd, bool i_bOwnHwnd, 
	int i_Width /*= -1*/, int i_Height /*= -1*/)
{
	m_bOwnHwnd = i_bOwnHwnd;

	int width = i_Width;
	int height = i_Height;
	if (width == -1 || height == -1)
	{
		RECT rect;
		::GetClientRect(i_Hwnd, &rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	g2dPFD pixel_format;
	initialize_window(i_Hwnd, width, height, pixel_format);

	// ** Setup base class info **

	// Give window properties to Window base
	bool windowed = true;
	g2dPFD backPFD;
	g2dDX11Global::PFDFromD3DFormat(m_BackBufferDesc.Format, backPFD);
	g2dWindow::SetWindowProperties(windowed, backPFD, backPFD,
		width, height, pixel_format.BitsPerPixel());
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dWindowPrimaryDX11::~g2dWindowPrimaryDX11()
{
	if (m_pSolidBrush2D)
		m_pSolidBrush2D->Release();
	if (m_pBackBuffer2D)
		m_pBackBuffer2D->Release();

	m_pBackBuffer->Release();
	m_pSwapChain->Release();

	if (m_bOwnHwnd)
		appApplication::DestroySubWindow(m_Hwnd);
}


//------------------------------------------------------------------------
// BeginScene must be called before rendering to this window
//------------------------------------------------------------------------
void g2dWindowPrimaryDX11::BeginScene()
{
	g3dStateMgrDX11::SetDefault();

	g2dWindowDX11::BeginScene();
	g2dDX11Global::BeginScene();
}

//------------------------------------------------------------------------
//	EndScene must be called when you are done with the drawing operations
//	on the current frame.  It will cause whatever drawing you have
//	requested to be visible on the screen.
//------------------------------------------------------------------------
void g2dWindowPrimaryDX11::EndScene()
{
	//if ( appApplication::IsSuspended() )
	//{
	//	return;
	//}

	g2dWindowDX11::EndScene();
	g2dDX11Global::EndScene();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g2dWindowPrimaryDX11::Present()
{
	//	do we need to reset the d3d device?
	HRESULT reset_result = m_pSwapChain->Present(0, 0);

	if (FAILED(reset_result))
	{
		switch (reset_result)
		{
		case DXGI_ERROR_DEVICE_RESET:
			DBG_ERROR("Present failed due to DXGI_ERROR_DEVICE_RESET");
			break;
		case DXGI_STATUS_OCCLUDED:
			DBG_ERROR("Present failed due to DXGI_STATUS_OCCLUDED");
			break;
		case DXGI_ERROR_DEVICE_REMOVED:
			{
				std::string deviceRemovedReason("unknown");
				ID3D11Device* pDevice = NULL;
				HRESULT hr = m_pSwapChain->GetDevice(__uuidof(pDevice), (void**)&pDevice);
				if (SUCCEEDED(hr))
				{
					HRESULT reason = pDevice->GetDeviceRemovedReason();
					switch (reason)
					{
					case E_OUTOFMEMORY :
						deviceRemovedReason = "E_OUTOFMEMORY"; break;
					case DXGI_ERROR_DEVICE_HUNG :
						deviceRemovedReason = "DXGI_ERROR_DEVICE_HUNG"; break;
					case DXGI_ERROR_DEVICE_REMOVED :
						deviceRemovedReason = "DXGI_ERROR_DEVICE_REMOVED"; break;
					case DXGI_ERROR_DEVICE_RESET : 
						deviceRemovedReason = "DXGI_ERROR_DEVICE_RESET"; break;
					case DXGI_ERROR_DRIVER_INTERNAL_ERROR :
						deviceRemovedReason = "DXGI_ERROR_DRIVER_INTERNAL_ERROR"; break;
					case DXGI_ERROR_INVALID_CALL :
						deviceRemovedReason = "DXGI_ERROR_INVALID_CALL"; break;
					}
					pDevice->Release();
				}
				DBG_ERROR("Present failed due to DXGI_ERROR_DEVICE_REMOVED, reason is " << deviceRemovedReason);
			}
			break;
		default:
			DBG_ERROR("Present failed due to error " << reset_result);
			break;
		}
	}
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g2dWindowPrimaryDX11::SetTitle(const itString& i_Title)
{
	SetWindowText(m_Hwnd, i_Title.GetString());
}

//------------------------------------------------------------------------
//	IsOpen returns true if the window is still open
//------------------------------------------------------------------------
bool g2dWindowPrimaryDX11::IsOpen() const
{
	return (IsWindow(m_Hwnd) != 0);
}

//------------------------------------------------------------------------
// Resize window to new size
//------------------------------------------------------------------------
void g2dWindowPrimaryDX11::ResizeWindow(int i_Width, int i_Height)
{
	// Release old buffers
	this->FreeBuffers();//this releases the depth buffer!!
	this->m_pBackBuffer->Release();

	DXGI_SWAP_CHAIN_DESC desc;
	HRESULT hr = m_pSwapChain->GetDesc(&desc);

	hr = m_pSwapChain->ResizeBuffers(desc.BufferCount, i_Width, i_Height, desc.BufferDesc.Format, desc.Flags);

	InitRenderTargetInfo();
	CreateDepthBuffer(i_Width, i_Height);

	// Give window properties to Window base
	bool windowed = true;
	g2dPFD backPFD;
	g2dDX11Global::PFDFromD3DFormat(m_BackBufferDesc.Format, backPFD);
	g2dWindow::SetWindowProperties(windowed, backPFD, backPFD,
		i_Width, i_Height, backPFD.BitsPerPixel());
}

//----------------------------------------------------------------------------
//	initialize_window initializes the drawing system to use a window
//	of the given width and height with the upper left corner at i_X and
//	i_Y.  If the user specifies invalid coordinates a
//	g2dUnsupportedScreenModeX exception will be thrown.
//----------------------------------------------------------------------------
void g2dWindowPrimaryDX11::initialize_window(HWND i_Hwnd, int i_Width, int i_Height, g2dPFD& o_PixelFormat)
{
	m_Hwnd = i_Hwnd;

	//	Let the HWND determine the window's size, so don't set it here
	appApplication::SetNonClientValid(false);

	DXGI_SWAP_CHAIN_DESC desc;
	setup_present_params(m_Hwnd, i_Width, i_Height, desc, g2dDX11Global::g_ScreenFormat);

	HRESULT hr = g2dDX11Global::g_pDXGIFactory->CreateSwapChain(g2dDX11Global::g_pDevice, &desc, &m_pSwapChain);
	if (FAILED(hr))
	{
		DBG_ERROR("Failed to create SwapChain for window.");
	}

	hr = InitRenderTargetInfo();
	hr = CreateDepthBuffer(i_Width, i_Height);
}

HRESULT g2dWindowPrimaryDX11::InitRenderTargetInfo()
{
	HRESULT hr;

	// Get the back buffer desc
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< void** >(&pBackBuffer) );
	if( FAILED( hr ) )
	{
		DBG_ERROR("Failed to get backbuffer of newly created window");
		return hr;
	}
    D3D11_TEXTURE2D_DESC backBufferSurfaceDesc;
	pBackBuffer->GetDesc( &backBufferSurfaceDesc );
	m_BackBufferDesc = backBufferSurfaceDesc;

    // Create the render target view
	g2dD3D11RenderTargetPtr pRTV = NULL;
	hr = g2dDX11Global::g_pDevice->CreateRenderTargetView( pBackBuffer, NULL, &pRTV );

	D3D_RELEASE( pBackBuffer );
	
	if( FAILED( hr ) )
		return hr;
	
	m_pBackBuffer = pRTV;



	// Get a surface in the swap chain
	IDXGISurface* pBackBufferSurf = NULL;
	hr = m_pSwapChain->GetBuffer( 0, IID_PPV_ARGS(&pBackBufferSurf));
    if (SUCCEEDED(hr))
    {
        // Create the DXGI Surface Render Target.
        FLOAT dpiX;
        FLOAT dpiY;
		g2dDX11Global::g_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

        D2D1_RENDER_TARGET_PROPERTIES props =
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
                dpiX,
                dpiY
                );

        // Create a Direct2D render target which can draw into the surface in the swap chain
        hr = g2dDX11Global::g_pD2DFactory->CreateDxgiSurfaceRenderTarget(
            pBackBufferSurf,
            &props,
            &m_pBackBuffer2D
            );

		if (SUCCEEDED(hr))
		{
			hr = m_pBackBuffer2D->CreateSolidColorBrush(
				 D2D1::ColorF(D2D1::ColorF::White),
				 &m_pSolidBrush2D);
		}

		pBackBufferSurf->Release();
    }

	return hr;
}

HRESULT g2dWindowPrimaryDX11::CreateDepthBuffer(int i_Width, int i_Height)
{
	m_DepthStencil.reset(new g2dDepthStencilBufferDX11());
	m_DepthStencil->Make(i_Width, i_Height, DXGI_FORMAT_D24_UNORM_S8_UINT, 
		g2dDX11Global::DefaultSampleDesc(), g2dResourceCounterDX11::eWindow);
	g2dDX11Global::g_bHasStencil = true;

	return S_OK;
}




