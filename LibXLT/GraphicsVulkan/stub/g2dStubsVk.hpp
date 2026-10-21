/*****************************************************************************
**  g2dStubsVk.hpp
**
**      Vulkan implementations of the 2D graphics hooks that aren't ported
**	yet: they keep MachStudio running (every call returns a usable object
**	or a harmless value) and log GVK_NOT_YET the first time a call that
**	should do real work runs. Each class moves to its real package when a
**	later plan ports it.
**
**	  g2dResourceCounterVk		memory statistics (all zero)
**
**	Images, image loading and saving, and capturing windows are ported:
**	G2d/g2dImageVk, G2d/g2dImageCreateVk, G2d/g2dImageSaveVk,
**	G2d/g2dScreenCaptureUtilVk.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_G2DSTUBSVK_HPP
#error g2dStubsVk.hpp multiply included
#endif
#define GVK_G2DSTUBSVK_HPP

#ifndef G2D_RESOURCECOUNTER_HPP
#include "Graphics/G2d/g2dResourceCounter.hpp"
#endif

namespace gvk
{
	class g2dResourceCounterVk : public g2dResourceCounterImpl
	{
		public:
			//	g2dResourceCounterDX11's categories: the texture and render
			//	target classes take one (counting comes with this class's port)
			enum eResourceCategory
			{
				eWindow,
				eRenderTarget,
				eShadow,
				eTexture
			};

			virtual float GetTotalVideoMemory();
			virtual float GetTotalSceneTextureMemory();
			virtual int GetNumSceneTextures();
			virtual float GetTotalFramebufferMemory();
			virtual int GetNumFramebufferTextures();
			virtual float GetTotalShadowMapMemory();
			virtual int GetNumShadowMapTextures();
			virtual float GetTotalVertexBufferMemory();
			virtual int GetNumVertexBuffers();
			virtual float GetTotalIndexBufferMemory();
			virtual int GetNumIndexBuffers();
	};
}
