/****************************************************************************\
**	g3dSceneRendererDX11.hpp
**
**	The g3dSceneRendererDX11 renders the g3dSceneNodes which describe the
**	graphical scene.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_SCENERENDERERDX11_HPP
#error g3dSceneRendererDX11.hpp multiply included
#endif
#define G3D_SCENERENDERERDX11_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif

#include <vector>

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class g3dLayer;
class camCamera;

class g3dSceneRendererDX11 : public g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	g3dSceneRendererDX11();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~g3dSceneRendererDX11();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
				 const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
				 float i_fSimTime );

	void ReleaseResources(){}

	static void InitStates();
	static void CleanupStates();
};

