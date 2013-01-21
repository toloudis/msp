#include "rndrGraphRenderer.h"

#include "rndrNode.h"

rndrGraphRenderer::rndrGraphRenderer(void)
{
}


rndrGraphRenderer::~rndrGraphRenderer(void)
{
}

int rndrGraphRenderer::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
			const g3dScene &i_Scene,
			const std::vector<g3dLayer*>& i_ViewerLayers,
			float i_fSimTime )
{
	mRoot->clearGraph(); //(?) or let classes clear specific nodes they want to?
	update(&i_Scene);
	mRoot->execute();
	return 0;
}

void rndrGraphRenderer::update(const g3dScene* i_Scene)
{
}
