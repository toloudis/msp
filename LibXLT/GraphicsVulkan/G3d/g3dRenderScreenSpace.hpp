/****************************************************************************\
**  g3dRenderScreenSpace.hpp
**
**      g3dRenderScreenSpace.hpp renders a later in screen space.
**
**	GraphicsVulkan port of GraphicsDX11's g3dRenderScreenSpaceObjects,
**	drawn by the simple renderer.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_G3D_RENDERSCREENSPACE_HPP
#error g3dRenderScreenSpace.hpp multiply included
#endif
#define GVK_G3D_RENDERSCREENSPACE_HPP

#ifndef GVK_G3D_RENDERPASS_HPP
#include "GraphicsVulkan/G3d/g3dRenderPass.hpp"
#endif

namespace gvk
{

//============================================================================
//============================================================================
class g3dRenderScreenSpaceObjects : public g3dRenderLayer
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	g3dRenderScreenSpaceObjects();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~g3dRenderScreenSpaceObjects(void);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual int Render(float i_time);

	static void InitStates();
	static void CleanupStates();
};

}
