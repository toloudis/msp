/****************************************************************************\
**  g2dDX11GlobalWin.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_DX11GLOBALWIN_HPP
#error g2dDX11GlobalWin.hpp multiply included
#endif
#define G2D_DX11GLOBALWIN_HPP

#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif
#ifndef ENV_THREAD_HPP
#include "Core/env/envThread.hpp"
#endif

#undef DrawText

#include <vector>

//============================================================================
//============================================================================
class g2dPFD;

//============================================================================
//============================================================================
namespace g2dDX11Global
{
	//----------------------------------------------------------------------------
	//	private PAC stuff
	//	These values are generally set once by the g2dScreen when it is set up.
	//	They should not be changed by any other code.  They are global only
	//	for fast "inline" access, not so they can be altered.  If you change
	//	these variables you will break everything.
	//----------------------------------------------------------------------------
	extern IDXGIFactory*		g_pDXGIFactory;
	extern IDXGIAdapter*		g_pAdapter;				// need array for multi-gpu	
	extern g2dD3D11DevicePtr	g_pDevice;				// The D3D 11 rendering device
	extern g2dD3D11DeviceContextPtr	g_pDeviceContext;	// The D3D 11 rendering device context
	extern bool					g_bHasStencil;			// true if we managed to get D24S8
	extern g2dPFD				g_ScreenPixelFormat;	// Screen pixel format
	extern DXGI_FORMAT			g_ScreenFormat;			// Screen D3D format
	extern g2dD3D11QueryPtr		g_pSynchronizationEvent;// use to flush rendering
	extern ID2D1Factory*		g_pD2DFactory;
	extern IDWriteFactory*		g_pDWriteFactory;

	extern envMutex g_D3DDeviceContextMutex;

	//--------------------------------------------------------------------
	// use to flush D3D rendering. USE ONLY IF EXPLICITLY NEEDED.
	//--------------------------------------------------------------------
	void Sync();

	//--------------------------------------------------------------------
	// global D3D state: Set the "current" render target.
	//--------------------------------------------------------------------
	void SetRenderTargets(g2dD3D11RenderTargetPtr i_renderTarget, g2dD3D11DepthStencilPtr i_depthStencil);
	void SetColorTarget(g2dD3D11RenderTargetPtr color);
	void SetDepthTarget(g2dD3D11DepthStencilPtr depth);

	//--------------------------------------------------------------------
	// global D3D state: Get the "current" render target.
	//--------------------------------------------------------------------
	g2dD3D11RenderTargetPtr GetColorTarget();
	g2dD3D11DepthStencilPtr GetDepthTarget();

	//--------------------------------------------------------------------
	// ideally we want only one BeginScene/EndScene pair before a Present() call
	//--------------------------------------------------------------------
	void BeginScene();
	void EndScene();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool RestrictPow2Textures();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void PFDFromD3DFormat(DXGI_FORMAT i_Format, g2dPFD& o_PFD);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	DXGI_FORMAT D3DFormatFromPFD(const g2dPFD& i_PFD);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	UINT BitsPerPixel( DXGI_FORMAT i_Format );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	bool IsFormatCompressed(DXGI_FORMAT i_Format);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void GetSurfaceInfo( UINT i_Width, UINT i_Height, DXGI_FORMAT i_Format, 
						UINT* o_pNumBytes, UINT* o_pRowBytes, UINT* o_pNumRows );

	//----------------------------------------------------------------------------
	//	PrintDXError dumps an error message to the debug log
	//----------------------------------------------------------------------------
	void PrintDXError(HRESULT hErr);

	//----------------------------------------------------------------------------
	// choose highest quality AA format available
	//----------------------------------------------------------------------------
	void ChooseMultisampleQuality(DXGI_FORMAT i_Format, UINT& o_MultiSampleCount, UINT& o_MultiSampleQuality);

	//----------------------------------------------------------------------------
	// choose defaul AA format (no AA)
	//----------------------------------------------------------------------------
	const DXGI_SAMPLE_DESC& DefaultSampleDesc();
}

