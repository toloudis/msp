/*****************************************************************************
**  matVkGlobal.cpp
**
**      See matVkGlobal.hpp.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_MATVKGLOBAL_HPP
#include "GraphicsVulkan/Mat/matVkGlobal.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif

namespace gvk
{
namespace matVkGlobal
{

namespace
{
	//	DX11 checks each with CheckFormatSupport; R8G8B8A8_UNORM sampling is
	//	required of every Vulkan device
	const g2dPFD& rgba8()
	{
		static g2dPFD s_PFD;
		static bool s_bMade = false;
		if (!s_bMade)
		{
			g2dVkGlobal::PFDFromDxgiFormat(g2dVkGlobal::e_DXGI_R8G8B8A8_UNORM, s_PFD);
			s_bMade = true;
		}
		return s_PFD;
	}
}

const g2dPFD& AlphaTextureFormat()			{ return rgba8(); }
const g2dPFD& NonAlphaTextureFormat()		{ return rgba8(); }
const g2dPFD& TrueAlphaTextureFormat()		{ return rgba8(); }
const g2dPFD& TrueNonAlphaTextureFormat()	{ return rgba8(); }

}
}
