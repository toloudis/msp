/****************************************************************************\
**  g2dDX11GlobalWin.cpp
**
**      g2dDX11GlobalWin.hpp defines some D3D stuff used by many other
**	components.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"

#include "Core/it/itStringUtil.hpp"
#include "Graphics/g2d/g2dPFD.hpp"

#include <vector>
//#include c_g2dDXERR_H
#include <sstream>

//============================================================================
//	library pragmas
//============================================================================
#pragma comment(lib,c_g2dDXGILIBRARY)
#pragma comment(lib,c_g2dD3D11LIBRARYMAIN)
#pragma comment(lib,c_g2dDXCOMPILER)
#pragma comment(lib,c_g2dD2D1LIBRARY)
#pragma comment(lib,c_g2dDWRITELIBRARY)
#pragma comment(lib,c_g2dDXGUIDLIBRARY_H)
#pragma comment(lib,"DirectXTex.lib")
#ifdef _DEBUG
    #pragma comment(lib,"Effects11.lib")
#else
    #pragma comment(lib,"Effects11.lib")
#endif
//============================================================================
//============================================================================
namespace g2dDX11Global
{
//----------------------------------------------------------------------------
//	private PAC stuff
//----------------------------------------------------------------------------
IDXGIFactory*		g_pDXGIFactory = NULL;
IDXGIAdapter*		g_pAdapter = NULL;				// need array for multi-gpu	
g2dD3D11DevicePtr	g_pDevice = NULL;				// The D3D 11 rendering device
g2dD3D11DeviceContextPtr g_pDeviceContext = NULL;	// The D3D 11 rendering device context
bool				g_bHasStencil = false;
g2dPFD				g_ScreenPixelFormat;			// Screen pixel format
DXGI_FORMAT			g_ScreenFormat;					// Screen D3D format

ID2D1Factory*		g_pD2DFactory = NULL;
IDWriteFactory*		g_pDWriteFactory = NULL;

// These are the "current" depth and color render targets.
g2dD3D11RenderTargetPtr g_curRenderTarget = NULL;
g2dD3D11DepthStencilPtr g_curDepthStencil = NULL;

envMutex g_D3DDeviceContextMutex;

//g2dD3DAlphaBlendState	g_blendState;
//g2dD3DZBufferState		g_zBufState;
//g2dD3DAlphaTestState	g_alphaTestState;

//--------------------------------------------------------------------
// use to flush rendering. possibly unnecessary.
//--------------------------------------------------------------------
g2dD3D11QueryPtr g_pSynchronizationEvent = NULL;

//--------------------------------------------------------------------
// use to flush D3D rendering. USE ONLY IF EXPLICITLY NEEDED.
//--------------------------------------------------------------------
void Sync()
{
	if (g_pSynchronizationEvent == NULL)
	{
		D3D11_QUERY_DESC desc;
		desc.Query = D3D11_QUERY_EVENT;
		desc.MiscFlags = 0;
		g_pDevice->CreateQuery(&desc, &g_pSynchronizationEvent);
	}

	// Add an end marker to the command buffer queue.
	g_pDeviceContext->End(g_pSynchronizationEvent);
	// Empty the command buffer and wait until the GPU is idle.
	BOOL queryData;
	while(S_OK != g_pDeviceContext->GetData(g_pSynchronizationEvent, &queryData, sizeof(BOOL), 0 ) )
	{
		//prevent infinite loop by issuing some arbitrary d3d graphics command.
		DBG_ASSERT(false, "D3D11 not implemented");
//?		g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT  );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CheckTargetFormatsMatch(g2dD3D11RenderTargetPtr i_renderTarget, g2dD3D11DepthStencilPtr i_depthStencil)
{
#if (ENV_DEBUG == 1)
//	DBG_LOG2("SetTargets %08x %08x", i_renderTarget, i_depthStencil);
	// assert that the formats are compatible
	if (i_renderTarget != NULL && i_depthStencil != NULL)
	{
//		D3DSURFACE_DESC sdesc;
//		i_renderTarget->GetDesc(&sdesc);
//		D3DFORMAT cFmt = sdesc.Format;
//		i_depthStencil->GetDesc(&sdesc);
//		D3DFORMAT dFmt = sdesc.Format;
//	    HRESULT result = g2dDX11Global::g_pD3D->CheckDepthStencilMatch(	D3DADAPTER_DEFAULT,
//			D3DDEVTYPE_HAL,
//			g_ScreenFormat,
//			cFmt,
//			dFmt);
//		DBG_ASSERT0(result == D3D_OK, "Incompatible renderTarget and depthStencil surfaces");
	}
#endif
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SetRenderTargets(g2dD3D11RenderTargetPtr i_renderTarget, g2dD3D11DepthStencilPtr i_depthStencil)
{
//	if (i_renderTarget != g_curRenderTarget)
	{
		CheckTargetFormatsMatch(i_renderTarget, i_depthStencil);
		g2dDX11Global::g_pDeviceContext->OMSetRenderTargets(1, &i_renderTarget, i_depthStencil);
		g_curRenderTarget = i_renderTarget;
		g_curDepthStencil = i_depthStencil;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SetColorTarget(g2dD3D11RenderTargetPtr i_renderTarget)
{
//	if (i_renderTarget != g_curRenderTarget)
	{
		CheckTargetFormatsMatch(i_renderTarget, g_curDepthStencil);
		g2dDX11Global::g_pDeviceContext->OMSetRenderTargets(1, &i_renderTarget, g_curDepthStencil);
		g_curRenderTarget = i_renderTarget;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SetDepthTarget(g2dD3D11DepthStencilPtr i_depthStencil)
{
//	if (i_depthStencil != g_curDepthStencil)
	{
		CheckTargetFormatsMatch(g_curRenderTarget, i_depthStencil);
		g2dDX11Global::g_pDeviceContext->OMSetRenderTargets( 1, &g_curRenderTarget, i_depthStencil );
		g_curDepthStencil = i_depthStencil;
	}
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g2dD3D11RenderTargetPtr GetColorTarget()
{
	return g_curRenderTarget;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g2dD3D11DepthStencilPtr GetDepthTarget()
{
	return g_curDepthStencil;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void BeginScene()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void EndScene()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
UINT BitsPerPixel( DXGI_FORMAT i_Format )
{
	switch( i_Format )
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return 128;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return 96;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		return 64;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8X8_TYPELESS:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		return 32;

	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		return 16;

	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
		return 8;

	case DXGI_FORMAT_R1_UNORM:
		return 1;

	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		return 4;

	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return 8;

	default:
		DBG_ASSERT(false, "Unknown DXGI format " << i_Format);
		DBG_WARNING("Unknown DXGI format " << i_Format);
		return 0;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool IsFormatCompressed(DXGI_FORMAT i_Format)
{
	switch (i_Format)
	{
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void GetSurfaceInfo( UINT i_Width, UINT i_Height, DXGI_FORMAT i_Format, 
					UINT* o_pNumBytes, UINT* o_pRowBytes, UINT* o_pNumRows )
{
	UINT numBytes = 0;
	UINT rowBytes = 0;
	UINT numRows = 0;

	bool bc = true;
	int bcnumBytesPerBlock = 16;
	switch (i_Format)
	{
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		bcnumBytesPerBlock = 8;
		break;

	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		break;

	default:
		bc = false;
		break;
	}
	// block compression is 4x4 blocks of pixels
	if( bc )
	{
		int numBlocksWide = 0;
		if( i_Width > 0 )
			numBlocksWide = max( 1, i_Width / 4 );
		int numBlocksHigh = 0;
		if( i_Height > 0 )
			numBlocksHigh = max( 1, i_Height / 4 );
		rowBytes = numBlocksWide * bcnumBytesPerBlock;
		numRows = numBlocksHigh;
	}
	else
	{
		UINT bpp = BitsPerPixel( i_Format );
		rowBytes = ( i_Width * bpp + 7 ) / 8; // round up to nearest byte
		numRows = i_Height;
	}
	numBytes = rowBytes * numRows;
	if( o_pNumBytes != NULL )
		*o_pNumBytes = numBytes;
	if( o_pRowBytes != NULL )
		*o_pRowBytes = rowBytes;
	if( o_pNumRows != NULL )
		*o_pNumRows = numRows;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void PFDFromD3DFormat(DXGI_FORMAT i_Format, g2dPFD& o_PFD)
{
	// set up some basic guess first
	o_PFD.SetPixelFormat(g2dPFD::e_Color);
	o_PFD.SetBitsPerPixel(BitsPerPixel( i_Format ));

	// now look for formats we want to specifically recognize
	switch( i_Format )
	{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			o_PFD.Set(	0,	8,
						8,	8,
						16,	8,
						24,	8,
						32);
		break;
		// TODO: this is a hack, the best approximation we've come up w/ for this
		// NOTE: this means that all DXT3 files are assumed to have alpha in them,
		//	whether they really do or not
		// DXT2 and DXT4 are the same as DXT3 and DXT5, except that they are 
		// interpreted as having premultiplied alpha in the rgb colors. 
		// We don't really support premultiplied alpha in our shading system.
		case DXGI_FORMAT_BC1_UNORM:  // fall through
		case DXGI_FORMAT_BC1_UNORM_SRGB:  // fall through
			o_PFD.Set(	16,	8,
						8,	8,
						0,	8,
						0,	0,
			// DXT compression results in 4 bpp storage.
						4);
			break;

		case DXGI_FORMAT_BC2_UNORM:  // fall through
		case DXGI_FORMAT_BC2_UNORM_SRGB:

		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
			o_PFD.Set(	16,	8,
						8,	8,
						0,	8,
						24,	8,
			// DXT compression results in 8 bpp storage.
						8);
			break;

		case DXGI_FORMAT_R16G16_SNORM:
			o_PFD.SetPixelFormat( g2dPFD::e_BumpMapV16U16 );
			o_PFD.SetBitsPerPixel(32);
			break;

		case DXGI_FORMAT_R8_UNORM:
			o_PFD.SetPixelFormat( g2dPFD::e_Luminance8 );
			o_PFD.SetBitsPerPixel(8);
			break;

		case DXGI_FORMAT_D16_UNORM:
			o_PFD.SetPixelFormat( g2dPFD::e_Depth16 );
			o_PFD.SetBitsPerPixel(16);
			break;

		case DXGI_FORMAT_R32_FLOAT:
			o_PFD.SetPixelFormat( g2dPFD::e_Float32 );
			o_PFD.SetBitsPerPixel(32);
			break;

		case DXGI_FORMAT_R16_FLOAT:
			o_PFD.SetPixelFormat( g2dPFD::e_Float16 );
			o_PFD.SetBitsPerPixel(16);
			break;

		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			o_PFD.SetPixelFormat( g2dPFD::e_RGBA16f );
			o_PFD.SetBitsPerPixel(64);
			o_PFD.Set(	0,	16,
						16,	16,
						32,	16,
						48,	16,
						64);
			break;
		case DXGI_FORMAT_R16G16B16A16_UNORM:
			o_PFD.SetPixelFormat( g2dPFD::e_RGBA16UInt );
			o_PFD.SetBitsPerPixel(64);
			o_PFD.Set(	0,	16,
						16,	16,
						32,	16,
						48,	16,
						64);
			break;
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			o_PFD.SetPixelFormat( g2dPFD::e_RGBA32f );
			o_PFD.SetBitsPerPixel(128);
			break;

		case DXGI_FORMAT_R32G32_FLOAT:
			o_PFD.SetPixelFormat( g2dPFD::e_GR32f );
			o_PFD.SetBitsPerPixel(64);
			break;

		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			o_PFD.SetPixelFormat( g2dPFD::e_Depth24Stencil8 );
			o_PFD.SetBitsPerPixel(32);
			break;

		case DXGI_FORMAT_D32_FLOAT:
			o_PFD.SetPixelFormat(g2dPFD::e_Depth32f);
			o_PFD.SetBitsPerPixel(32);
			break;

		case DXGI_FORMAT_B8G8R8X8_UNORM:
			o_PFD.SetPixelFormat( g2dPFD::e_RColor );
			o_PFD.SetBitsPerPixel(32);
			break;

		default:
			o_PFD.SetPixelFormat( g2dPFD::e_Unknown );
			DBG_WARNING("PFDFromD3DFormat: Unsupported D3D texure format " << i_Format);
		break;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
DXGI_FORMAT D3DFormatFromPFD(const g2dPFD& i_PFD)
{
	int format = i_PFD.GetPixelFormat();

	if ( format == g2dPFD::e_Color )
	{
		switch( i_PFD.BitsPerPixel() )
		{
			case 32:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		}
	}
	if ( format == g2dPFD::e_RColor )
	{
		switch( i_PFD.BitsPerPixel() )
		{
		case 32:
			return DXGI_FORMAT_B8G8R8X8_UNORM;
			break;
		}
	}
	else if ( format == g2dPFD::e_BumpMapV8U8 )
	{
		return 	DXGI_FORMAT_R8G8_SNORM;
	}
	else if ( format == g2dPFD::e_BumpMapV16U16 )
	{
		return DXGI_FORMAT_R16G16_SNORM;
	}
	else if (format == g2dPFD::e_RGBA16f )
	{
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	}
	else if (format == g2dPFD::e_RGBA16UInt)
	{
		return DXGI_FORMAT_R16G16B16A16_UNORM;
	}
	else if (format == g2dPFD::e_RGBA32f )
	{
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	else if (format == g2dPFD::e_GR32f )
	{
		return DXGI_FORMAT_R32G32_FLOAT;
	}
	else if (format == g2dPFD::e_Float16 )
	{
		return DXGI_FORMAT_R16_FLOAT;
	}
	else if (format == g2dPFD::e_Float32 )
	{
		return DXGI_FORMAT_R32_FLOAT;
	}
	else if (format == g2dPFD::e_Luminance8 )
	{
		return DXGI_FORMAT_R8_UNORM;
	}
	

	DBG_ERROR("Unknown g2dPFD format " << format);
	return DXGI_FORMAT_UNKNOWN;
}

//----------------------------------------------------------------------------
//	PrintDXError dumps an error message to the debug log
//----------------------------------------------------------------------------
void PrintDXError( HRESULT hErr )
{
//    D3D11_ERROR_FILE_NOT_FOUND	The file was not found.
//    D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS	There are too many unique instances of a particular type of state object.
//    D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS	There are too many unique instances of a particular type of view object.
//    D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD	The first call to ID3D11DeviceContext::Map after either ID3D11Device::CreateDeferredContext or ID3D11DeviceContext::FinishCommandList per Resource was not D3D11_MAP_WRITE_DISCARD.
//    D3DERR_INVALIDCALL(replaced with DXGI_ERROR_INVALID_CALL)	The method call is invalid.For example, a method's parameter may not be a valid pointer.
//    D3DERR_WASSTILLDRAWING(replaced with DXGI_ERROR_WAS_STILL_DRAWING)	The previous blit operation that is transferring information to or from this surface is incomplete.
//    E_FAIL	Attempted to create a device with the debug layer enabled and the layer is not installed.
//    E_INVALIDARG	An invalid parameter was passed to the returning function.
//    E_OUTOFMEMORY	Direct3D could not allocate sufficient memory to complete the call.
//    E_NOTIMPL	The method call isn't implemented with the passed parameter combination.
//    S_FALSE	Alternate success value, indicating a successful but nonstandard completion(the precise meaning depends on context).

		LPVOID lpMsgBuf;

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			hErr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );

    std::wstringstream dderr;
    switch (hErr)
    {
	case D3D11_ERROR_FILE_NOT_FOUND: 

		case E_FAIL:
			dderr << L"E_FAIL";
		break;
		case E_INVALIDARG:
			dderr << L"E_INVALIDARG";
		break;
		case E_OUTOFMEMORY:
			dderr << L"E_OUTOFMEMORY";
		break;
		default:
			dderr << L"[" << (lpMsgBuf ? ((LPCTSTR)lpMsgBuf) : L"") << L"] (" << std::hex << hErr << L")";
		break;
	}

	DBG_WARNING("DirectX Error: " << itStringUtil::GetStdString(itString(dderr.str().c_str())));
		LocalFree(lpMsgBuf);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool RestrictPow2Textures()
{
	return false;
	// if either of these caps flags are set, then we must enforce power of 2 texture sizes.
//	return ((g_Caps.TextureCaps & D3DPTEXTURECAPS_POW2) || 
//		(g_Caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL));
}

//----------------------------------------------------------------------------
// choose highest quality AA format available
//----------------------------------------------------------------------------
void ChooseMultisampleQuality(DXGI_FORMAT i_Format, UINT& o_MultiSampleCount, UINT& o_MultiSampleQuality)
{
	// in DX11, feature level 11, we should get at least 8x msaa for any format below r32g32b32a32 

	// find maximum multisample level.
	o_MultiSampleQuality = 0;
	o_MultiSampleCount = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT;//D3DMULTISAMPLE_NONMASKABLE;//;
	do
	{
		g2dDX11Global::g_pDevice->CheckMultisampleQualityLevels(
			i_Format, o_MultiSampleCount, &o_MultiSampleQuality );
		if( o_MultiSampleQuality != 0 )
		{
			break;	//found a match
		}
		else
		{
			o_MultiSampleCount >>= 1;
		}
	} 
	while( o_MultiSampleCount >= 1 );

	o_MultiSampleQuality--;

	if( !o_MultiSampleCount )
	{
		DBG_WARNING("Multisampled rendertarget requested but multisampling not supported.");
	}
}

//----------------------------------------------------------------------------
// choose defaul AA format (no AA)
//----------------------------------------------------------------------------
const DXGI_SAMPLE_DESC& DefaultSampleDesc()
{
	static DXGI_SAMPLE_DESC s_NoMSAA = {1,0};
	return s_NoMSAA;
}

}  //namespace

