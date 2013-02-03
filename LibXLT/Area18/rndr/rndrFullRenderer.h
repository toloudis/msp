#pragma once
#include "Graphics\g3d\g3dscenerenderer.hpp"

#include "Graphics/G3d/g3dSceneNode.hpp"
class g3dSceneNode;

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

private:
	void TraverseLayer(float i_time, const g3dLayer* i_pLayer, const camCamera* i_pCamera, bool i_bDoClipping);
	void Traverse( g3dSceneNode* i_pNode, 
			  g3dSceneNode::DrawStyle i_DrawStyle,
			  bool i_bRenderLowRes,
			  bool i_bDoClip);

	std::vector<g3dSceneNode*> mNodesToDraw;

};

