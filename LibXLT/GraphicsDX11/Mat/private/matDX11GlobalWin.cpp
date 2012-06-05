/****************************************************************************\
**  matDX11GlobalWin.hpp
**
**      matDX11GlobalWin.hpp contains some D3D stuff that many components
**	in the Windows PAC for mat might need.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/mat/matDX11GlobalWin.hpp"

#include "Core/app/appFlowEventHandler.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "Graphics/g2d/g2dPFD.hpp"
#include "Graphics/mat/matExceptionX.hpp"

#include <vector>

namespace matD3DGlobal
{

namespace
{
bool l_INTZ_Supported = false;
bool l_NULL_Supported = false;

g2dPFD l_AlphaFormat;
g2dPFD l_NonAlphaFormat;
g2dPFD l_TrueAlphaFormat;
g2dPFD l_TrueNonAlphaFormat;

void get_texture_formats()
{
	HRESULT op_result;

	DXGI_FORMAT AlphaFormat, NonAlphaFormat, TrueAlphaFormat, TrueNonAlphaFormat;

	UINT formatSupport = 0;
	UINT textureSupport = D3D11_FORMAT_SUPPORT_TEXTURE2D;

	AlphaFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	op_result = g2dDX11Global::g_pDevice->CheckFormatSupport(AlphaFormat, &formatSupport);
	if( !SUCCEEDED(op_result) || !(formatSupport & textureSupport))
	{
		//	hmm, no acceptable format...fail
		g2dPFD pfd;
		g2dDX11Global::PFDFromD3DFormat(AlphaFormat, pfd);
		throw matUnsupportedPixelFormatX(pfd);
	}

	NonAlphaFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	op_result = g2dDX11Global::g_pDevice->CheckFormatSupport(NonAlphaFormat, &formatSupport);
	if( !SUCCEEDED(op_result) || !(formatSupport & textureSupport))
	{
		g2dPFD pfd;
		g2dDX11Global::PFDFromD3DFormat(NonAlphaFormat, pfd);
		throw matUnsupportedPixelFormatX(pfd);
	}

	TrueAlphaFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	op_result = g2dDX11Global::g_pDevice->CheckFormatSupport(TrueAlphaFormat, &formatSupport);
	if( !SUCCEEDED(op_result) || !(formatSupport & textureSupport))
	{
		TrueAlphaFormat = AlphaFormat;
	}

	//	find a hi-color non-alpha format
	//	try	888 first
	TrueNonAlphaFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	op_result = g2dDX11Global::g_pDevice->CheckFormatSupport(TrueNonAlphaFormat, &formatSupport);
	if( !SUCCEEDED(op_result) || !(formatSupport & textureSupport))
	{
		TrueNonAlphaFormat = NonAlphaFormat;
	}

	//	setup pfds
	g2dDX11Global::PFDFromD3DFormat(AlphaFormat, l_AlphaFormat);
	g2dDX11Global::PFDFromD3DFormat(NonAlphaFormat, l_NonAlphaFormat);
	g2dDX11Global::PFDFromD3DFormat(TrueAlphaFormat, l_TrueAlphaFormat);
	g2dDX11Global::PFDFromD3DFormat(TrueNonAlphaFormat, l_TrueNonAlphaFormat);
}


}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Initialize()
{
	get_texture_formats();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DeInitialize()
{
}

//----------------------------------------------------------------------------
//	AlphaTextureFormat is the pixel format which will be used for all textures
//	which contain alpha information.
//----------------------------------------------------------------------------
const g2dPFD& AlphaTextureFormat()
{
	return l_AlphaFormat;
}

//----------------------------------------------------------------------------
//	AlphaTextureFormat is the pixel format which will be used for all textures
//	which don't contain alpha information.
//----------------------------------------------------------------------------
const g2dPFD& NonAlphaTextureFormat()
{
	return l_NonAlphaFormat;
}

//----------------------------------------------------------------------------
//	TrueAlphaTextureFormat is a format which is not meant be used for display
//	but can be used as an intermediate for mip-map generation and so on, to
//	provide higher quality.
//----------------------------------------------------------------------------
const g2dPFD& TrueAlphaTextureFormat()
{
	return l_TrueAlphaFormat;
}

//----------------------------------------------------------------------------
//	TrueNonAlphaTextureFormat is a format which is not meant be used for display
//	but can be used as an intermediate for mip-map generation and so on, to
//	provide higher quality.
//----------------------------------------------------------------------------
const g2dPFD& TrueNonAlphaTextureFormat()
{
	return l_TrueNonAlphaFormat;
}


}
