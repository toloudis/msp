/*****************************************************************************
**  matStubsVk.cpp
**
**      See matStubsVk.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_MATSTUBSVK_HPP
#include "GraphicsVulkan/stub/matStubsVk.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif


namespace gvk
{
	//------------------------------------------------------------------------
	//	matStubTextureVk
	//------------------------------------------------------------------------
	matStubTextureVk::matStubTextureVk(int i_Width, int i_Height, const g2dPFD* i_pPFD, const std::string& i_Name)
		: m_Name(i_Name)
	{
		SetWidth(i_Width);
		SetHeight(i_Height);
		SetPixelFormat(i_pPFD ? *i_pPFD : g2dPFD(0, 8, 8, 8, 16, 8, 24, 8, 32));
	}

	matStubTextureVk::~matStubTextureVk()
	{
	}
}
