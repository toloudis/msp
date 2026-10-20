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
	shdwPassFog::shdwPassFog(g2dRenderTarget*, matRenderTargetTexture*, matRenderTargetTexture*, const g3dScene*, const camCamera*)
	{
	}
	int shdwPassFog::Render(float)
	{
		GVK_NOT_YET("shdwPassFog");
		return 0;
	}

	shdwPassDOF::shdwPassDOF()
	{
	}
	int shdwPassDOF::Render(float)
	{
		GVK_NOT_YET("shdwPassDOF");
		return 0;
	}

	shdwPassAOVolumes::shdwPassAOVolumes(g2dRenderTarget*, matRenderTargetTexture*, matRenderTargetTexture*,
		matRenderTargetTexture*, matRenderTargetTexture*, const camCamera*, const ssaoParams&)
	{
	}
	int shdwPassAOVolumes::Render(float)
	{
		GVK_NOT_YET("shdwPassAOVolumes");
		return 0;
	}

	shdwPassSSGI::shdwPassSSGI(matRenderTargetTexture*, matRenderTargetTexture*, matRenderTargetTexture*, g2dRenderTarget*,
		matRenderTargetTexture*, matRenderTargetTexture*, const ssgiParams&, matRenderTargetTexture*)
	{
	}
	int shdwPassSSGI::Render(float)
	{
		GVK_NOT_YET("shdwPassSSGI");
		return 0;
	}

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
