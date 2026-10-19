/****************************************************************************\
**  g3dVkTextureUtil.hpp
**
**	GraphicsVulkan port of GraphicsDX11's g3dDX11TextureUtil: the image of
**	a Terawatt texture (GetD3DTexture's counterpart). The rest of
**	g3dDX11TextureUtil (texture rectangles, blur offsets) comes with the
**	renderers that use it.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G3DVKTEXTUREUTIL_HPP
#error g3dVkTextureUtil.hpp multiply included
#endif
#define GVK_G3DVKTEXTUREUTIL_HPP

class matTexture;

namespace gvk
{
	class vkdImage;

	namespace g3dVkTextureUtil
	{
		//------------------------------------------------------------------------
		// GetImage - the image shaders read for a Terawatt texture (the first
		// page of a UVA texture); NULL if it has none
		//------------------------------------------------------------------------
		vkdImage* GetImage(const matTexture* i_pTexture);
	}
}
