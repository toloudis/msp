/****************************************************************************\
**  matDX11GlobalWin.hpp
**
**      matDX11GlobalWin.hpp contains some D3D stuff that many components
**	in the Windows PAC for mat might need.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_DX11GLOBALWIN_HPP
#error matDX11GlobalWin.hpp multiply included
#endif
#define MAT_DX11GLOBALWIN_HPP

class g2dPFD;

namespace matD3DGlobal
{

//----------------------------------------------------------------------------
//	Initialize causes this component to gather information about the
//	texture formats which should be used.  This must be done after the
//	d3d device is created.
//----------------------------------------------------------------------------
void Initialize();

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DeInitialize();

//----------------------------------------------------------------------------
//	AlphaTextureFormat is the pixel format which will be used for all textures
//	which contain alpha information.
//----------------------------------------------------------------------------
const g2dPFD& AlphaTextureFormat();

//----------------------------------------------------------------------------
//	AlphaTextureFormat is the pixel format which will be used for all textures
//	which don't contain alpha information.
//----------------------------------------------------------------------------
const g2dPFD& NonAlphaTextureFormat();

//----------------------------------------------------------------------------
//	TrueAlphaTextureFormat is a format which is not meant be used for display
//	but can be used as an intermediate for mip-map generation and so on, to
//	provide higher quality.
//----------------------------------------------------------------------------
const g2dPFD& TrueAlphaTextureFormat();

//----------------------------------------------------------------------------
//	TrueNonAlphaTextureFormat is a format which is not meant be used for display
//	but can be used as an intermediate for mip-map generation and so on, to
//	provide higher quality.
//----------------------------------------------------------------------------
const g2dPFD& TrueNonAlphaTextureFormat();

}


