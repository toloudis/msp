/*****************************************************************************
**  matVkGlobal.hpp
**
**	GraphicsVulkan port of GraphicsDX11's matD3DGlobal (matDX11GlobalWin):
**	the pixel formats textures are made with when the caller gives none.
**	All four are R8G8B8A8_UNORM, as on DX11 (every Vulkan device samples
**	and renders it).
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_MATVKGLOBAL_HPP
#error matVkGlobal.hpp multiply included
#endif
#define GVK_MATVKGLOBAL_HPP

class g2dPFD;

namespace gvk
{
namespace matVkGlobal
{

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
}
