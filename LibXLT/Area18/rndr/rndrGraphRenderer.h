#pragma once

#include "Graphics/g3d/g3dSceneRenderer.hpp"

class rndrNode;

class rndrGraphRenderer : public g3dSceneRenderer
{
public:
	rndrGraphRenderer(void);
	virtual ~rndrGraphRenderer(void);

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy plus additional 
	//	viewer specific layers.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	virtual int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
			 const g3dScene &i_Scene,
			 const std::vector<g3dLayer*>& i_ViewerLayers,
			 float i_fSimTime );
protected:
	rndrNode* mRoot;

	virtual void update(const g3dScene* i_Scene);
};

