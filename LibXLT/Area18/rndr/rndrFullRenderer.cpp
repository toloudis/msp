#include "rndrFullRenderer.h"

#include "Area18/ogl/oglTypes.hpp"

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
int rndrFullRenderer::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
						const g3dScene &i_Scene,
						const std::vector<g3dLayer*>& i_ViewerLayers,
						float i_fSimTime )
{
	glClearColor(1,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);
	return 0;
}

//--------------------------------------------------------------------
//	Let a renderer free up any memory it is holding on to 
//--------------------------------------------------------------------
void rndrFullRenderer::ReleaseResources()
{
}

