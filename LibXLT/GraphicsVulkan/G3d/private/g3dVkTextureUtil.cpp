/****************************************************************************\
**  g3dVkTextureUtil.cpp
**
**	See g3dVkTextureUtil.hpp.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_MATTEXTUREVK_HPP
#include "GraphicsVulkan/Mat/matTextureVk.hpp"
#endif
#ifndef MAT_UVATEXTURE_HPP
#include "Graphics/Mat/matUVATexture.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

namespace gvk
{
	//------------------------------------------------------------------------
	// GetImage - return the image for Terawatt texture
	//------------------------------------------------------------------------
	vkdImage* g3dVkTextureUtil::GetImage(const matTexture* i_pTexture)
	{
		const matUVATexture* uva = dynamic_cast<const matUVATexture*>(i_pTexture);
		if ( uva != NULL )
		{
			// only supports 1 tex-page for now
			const matTextureVk* cur_page = dynamic_cast<const matTextureVk*>(uva->GetPage(0));
			DBG_ASSERT(cur_page, "UVA texture page is not a valid Vulkan texture.");
			return cur_page ? cur_page->GetImage() : NULL;
		}

		const matTextureVk* texture = dynamic_cast<const matTextureVk*>(i_pTexture);
		if( texture != NULL )
		{
			return texture->GetImage();
		}

		return NULL;
	}
}
