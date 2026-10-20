/*****************************************************************************
**  shdwPassStubsVk.cpp
**
**      See shdwPassStubsVk.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_SHDW_PASSSTUBSVK_HPP
#include "GraphicsVulkan/stub/shdwPassStubsVk.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif

namespace gvk
{
	shdwPassLPVGI::shdwPassLPVGI(const ssgiParams&, matRenderTargetTexture*, matRenderTargetTexture*, g2dRenderTarget*,
		const camCamera*, const g3dScene*, bool)
	{
	}
	int shdwPassLPVGI::Render(float)
	{
		GVK_NOT_YET("shdwPassLPVGI");
		return 0;
	}
}
