/****************************************************************************\
**  g3dRenderScreenSpace.hpp
**
**      g3dRenderScreenSpace.hpp renders a later in screen space.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_RENDERSCREENSPACE_HPP
#error g3dRenderScreenSpace.hpp multiply included
#endif
#define G3D_RENDERSCREENSPACE_HPP

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

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
