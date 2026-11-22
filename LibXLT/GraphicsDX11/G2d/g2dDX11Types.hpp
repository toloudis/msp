/****************************************************************************\
**  g2dDX11Types.hpp
**
**      Defines some D3D typedefs to hide the specific D3D version being 
**	used.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_DX11TYPES_HPP
#error g2dDX11Types.hpp multiply included
#endif
#define G2D_DX11TYPES_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif


//
//		DX11
//
//#if (ENV_GRAPHICLIBRARY == ENV_GLIB_DX11)

//#ifdef ENV_DEBUG
//	#define D3D_DEBUG_INFO
//#endif
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
//#include <d3dx11.h>

// 3rdparty for now.
// https://github.com/Microsoft/DirectXTex
#include <DirectXTex.h>
//#include <d3dx11.h>

#ifndef D3DPERF_BeginEvent
#define	D3DPERF_BeginEvent( C, S )
#endif

#ifndef D3DPERF_EndEvent
#define D3DPERF_EndEvent()
#endif

#ifndef D3DPERF_SetMarker
#define D3DPERF_SetMarker( C, S )
#endif
	
#undef DrawText

#include <D2D1.h>
#include <DWrite.h>

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif  

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

typedef IDXGISurface*				g2DDXGISurfacePtr;
typedef IDXGISwapChain*				g2DDXGISwapChainPtr;

typedef ID3D11DepthStencilView*		g2dD3D11DepthStencilPtr;
typedef ID3D11RenderTargetView*		g2dD3D11RenderTargetPtr;
typedef ID3D11ShaderResourceView*	g2dD3D11ShaderResourcePtr;

typedef ID3D11Resource*				g2dD3D11ResourcePtr;
typedef ID3D11Texture2D*			g2dD3D11TexturePtr;
typedef ID3D11Texture3D*			g2dD3D11VolumeTexturePtr;
//typedef ID3D11TextureCube*			g2dD3D11CubeTexturePtr;
typedef ID3D11Buffer*				g2dD3D11VertexBufferPtr;
typedef ID3D11Buffer*				g2dD3D11IndexBufferPtr;
typedef ID3D11Device*				g2dD3D11DevicePtr;
typedef ID3D11DeviceContext*		g2dD3D11DeviceContextPtr;
typedef ID3D11Query*				g2dD3D11QueryPtr;

typedef ID3D11VertexShader			g2dIDirect3DVertexShader10;
typedef ID3D11InputLayout			g2dIDirect3DVertexDeclaration10;
typedef ID3D11PixelShader			g2dIDirect3DPixelShader10;

#define g2dIID_ID3D11Resource		IID_ID3D11Resource 
#define g2dIID_ID3D11Texture2D		IID_ID3D11Texture2D 
#define g2dIID_ID3D11Texture3D		IID_ID3D11Texture3D

#define	c_g2dD3D11LIBRARYMAIN		"d3d11.lib"
#define c_g2dDXGILIBRARY			"dxgi.lib"

#define c_g2dDXCOMPILER				"d3dcompiler.lib"
#define c_g2dD2D1LIBRARY			"d2d1.lib"
#define c_g2dDWRITELIBRARY			"dwrite.lib"
#define c_g2dDXGUIDLIBRARY_H		"dxguid.lib"


//d3dcompiler.lib dxerr.lib dxguid.lib dxgi.lib d3d11.lib  winmm.lib comctl32.lib

#define c_g2dD3D11_H			"d3d11.h"
#define c_g2dDXERR_H			"dxerr.h"


#define D3D_RELEASE(ptr) {if (ptr)ptr->Release();ptr=NULL;}


