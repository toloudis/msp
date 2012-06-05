/****************************************************************************\
**	g3dCubicReflectionTargetRenderer.hpp
**
**		A g3dCubicReflectionTargetRenderer is a convenience class for rendering a scene
**	to a texture. It does not maintain a debug display like the g3dViewer class.
**
**		Note: this base class does not own any of its pointers.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_CUBICREFLECTIONTARGETRENDERER_HPP
#error g3dCubicReflectionTargetRenderer.hpp already included
#endif
#define G3D_CUBICREFLECTIONTARGETRENDERER_HPP

#ifndef G3D_TARGETRENDERER_HPP
#include "Graphics/g3d/g3dTargetRenderer.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class camCamera;
class g2dRenderTarget;
class g3dSceneRenderer;
class g3dScene;


//============================================================================
//============================================================================
class g3dCubicReflectionTargetRenderer : public g3dTargetRenderer
{
public:
	//----------------------------------------------------------------------------
	// The root node is not owned by the layer, just pointed to
	//----------------------------------------------------------------------------
	g3dCubicReflectionTargetRenderer(g2dRenderTarget* i_pTarget, g3dSceneRenderer* i_pRenderer,
		g3dScene* i_pScene, camCamera* i_pCamera);

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//--------------------------------------------------------------------
	virtual void Render( float i_fSimTime, 
						 const std::vector<g3dLayer*>& i_ViewerLayers, 
						 bool i_bClear = true );
};

