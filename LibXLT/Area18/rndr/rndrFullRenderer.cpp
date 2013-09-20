#include "rndrFullRenderer.h"

#include "Area18/ogl/oglTypes.hpp"
#include "Area18/ogl/oglView.h"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/G2d/g2dRenderTarget.hpp"
#include "Graphics/G3d/g3dFragment.hpp"
#include "Graphics/G3d/g3dLayer.hpp"
#include "Graphics/G3d/g3dScene.hpp"

rndrFullRenderer::rndrFullRenderer(void)
{
}


rndrFullRenderer::~rndrFullRenderer(void)
{
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy plus additional 
//	viewer specific layers.
//	Returns the number of triangles rendered
//--------------------------------------------------------------------
int rndrFullRenderer::Render( g2dRenderTarget* iWindow, const camCamera& i_Camera,
						const g3dScene &i_Scene,
						const std::vector<g3dLayer*>& i_ViewerLayers,
						float i_fSimTime )
{
	glClearColor(1,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);
	GLsizei w,h;
	iWindow->GetDimensions(w,h);
	glViewport(0,0,w,h);

	// camera setup for gl:
	//oglView v;
	//v.setAspectRatio(i_Camera.GetAspect());
	//v.setFarClipDistance(i_Camera.GetFarClip());
	//v.setNearClipDistance(i_Camera.GetNearClip());
	//v.setFieldOfView(i_Camera.GetFOV()*3.14159265/180.0);
	//if (i_Camera.IsOrthographic()) {
	//	// TODO: handle ortho conversion
	//	//v.setOrthoProjection(i_Camera.GetLeft(), i_Camera.GetOrthoWidth()
	//}
	//else {
	//	v.setPerspectiveProjection(i_Camera.GetFOV());// check units and x/y dimension
	//}
	//float t,b,l,r;
	//i_Camera.GetSubViewport(t,b,l,r);
	//v.setScreenWindow(l,r,b,t);
	
	// traverse scene
	mNodesToDraw.clear();
	for (int i = 0; i < i_Scene.GetNumLayers(); ++i) {
		TraverseLayer(i_fSimTime, i_Scene.GetLayer(i),
			&i_Camera, false);
	}

	// draw from list.


	return 0;
}

//--------------------------------------------------------------------
//	Let a renderer free up any memory it is holding on to 
//--------------------------------------------------------------------
void rndrFullRenderer::ReleaseResources()
{
}

void rndrFullRenderer::Traverse( g3dSceneNode* i_pNode, 
			  g3dSceneNode::DrawStyle i_DrawStyle,
			  bool i_bRenderLowRes,
			  bool i_bDoClip)
{ 
	const g3dFragment* pFrag = i_pNode->GetFragment();

	// Check if it has geometry
	if( pFrag && !pFrag->IsShadowHull())
	{
		matMaterial* pMatOverride = i_pNode->GetMaterial();
		mNodesToDraw.push_back(i_pNode);
	}

	// Render the children
	std::vector<g3dSceneNode*>& children = i_pNode->GetChildren();
	int nkids = children.size();
	for (int i=0; i<nkids; i++)
		Traverse(children[i], 
			i_DrawStyle,
			i_bRenderLowRes,
			i_bDoClip);
}

void rndrFullRenderer::TraverseLayer(float i_time, const g3dLayer* i_pLayer,
	const camCamera* i_pCamera, bool i_bDoClipping)
{
	g3dSceneNode::DrawStyle draw_style = g3dSceneNode::e_Inherit;

	bool bLowRes = false;
	Traverse(i_pLayer->GetRootNode(),
		draw_style,
		bLowRes,
		i_bDoClipping);
}


