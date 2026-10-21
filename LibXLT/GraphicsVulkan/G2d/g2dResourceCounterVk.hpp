/****************************************************************************\
**  g2dResourceCounterVk.hpp
**
**	g2dResourceCounterDX11 on Vulkan: the KB and number of the textures,
**	render targets, shadow maps and window buffers that the g2d texture,
**	render-target texture, depth-stencil and render-target classes make
**	(each adds what DX11's computes for the same size and format, by the
**	same rule), plus g3dVkBufferUtil's vertex and index buffer totals.
**	It counts what DX11 counts, not what Vulkan allocates, so the memory
**	report means the same on both backends.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G2DRESOURCECOUNTERVK_HPP
#error g2dResourceCounterVk.hpp multiply included
#endif
#define GVK_G2DRESOURCECOUNTERVK_HPP

#ifndef G2D_RESOURCECOUNTER_HPP
#include "Graphics/G2d/g2dResourceCounter.hpp"
#endif

namespace gvk
{
	//--------------------------------------------------------------------
	// all memory counts in KB
	//--------------------------------------------------------------------
	class g2dResourceCounterVk : public g2dResourceCounterImpl
	{
		public:
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

			//--------------------------------------------------------------------
			// add a resource not managed by matTextureMgr
			//--------------------------------------------------------------------
			static void AddResource(float i_SizeKBytes, eResourceCategory i_Category);
			static void RemoveResource(float i_SizeKBytes, eResourceCategory i_Category);
	};
}
