#pragma once
#include "Graphics\g3d\g3dscenerenderer.hpp"
class rndrFullRenderer :
	public g3dSceneRenderer
{
public:
	rndrFullRenderer(void);
	virtual ~rndrFullRenderer(void);

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy plus additional 
	//	viewer specific layers.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	virtual int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
						 const g3dScene &i_Scene,
						 const std::vector<g3dLayer*>& i_ViewerLayers,
						 float i_fSimTime );

	//--------------------------------------------------------------------
	//	Let a renderer free up any memory it is holding on to 
	//--------------------------------------------------------------------
	virtual void ReleaseResources();

};

