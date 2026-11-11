/*****************************************************************************
**  g2dResourceCounterOptiX.hpp
**
**      g2dResourceCounterOptiX reports the OptiX backend's GPU memory by
**	category (g2dResourceCounter; all memory counts in KB).
**
**	Stub: everything is 0.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G2DRESOURCECOUNTEROPTIX_HPP
#error g2dResourceCounterOptiX.hpp multiply included
#endif
#define GOX_G2DRESOURCECOUNTEROPTIX_HPP

#ifndef G2D_RESOURCECOUNTER_HPP
#include "Graphics/G2d/g2dResourceCounter.hpp"
#endif

namespace gox
{
	class g2dResourceCounterOptiX : public g2dResourceCounterImpl
	{
		public:
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
