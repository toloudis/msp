/****************************************************************************\
**	g3dCubicReflectionTargetRenderer.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dCubicReflectionTargetRenderer.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/g2d/g2dRenderTarget.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"


//----------------------------------------------------------------------------
// The root node is not owned by the layer, just pointed to
//----------------------------------------------------------------------------
g3dCubicReflectionTargetRenderer::g3dCubicReflectionTargetRenderer(g2dRenderTarget* i_pTarget, 
																   g3dSceneRenderer* i_pRenderer,
																   g3dScene* i_pScene, 
																   camCamera* i_pCamera)
:	g3dTargetRenderer(i_pTarget, i_pRenderer, i_pScene, i_pCamera)
{
	camPassBuffersData passBuffersData;
	//i_pCamera->GetPassBuffersParams(passBuffersData);
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
void g3dCubicReflectionTargetRenderer::Render( float i_fSimTime, 
											   const std::vector<g3dLayer*>& i_ViewerLayers, 
											   bool i_bClear )
{
	if (!m_pRenderer->IsEnabled())
		return;

	m_NumPrimitivesRendered = 0;

	g2dRGBColor bg_color = m_BackgroundColor;

	// if fog is enabled, set background color to fog's color
	// [dmt] - disregard fog for reflection renderers for now.
//	if (m_pScene->IsFogEnabled())
//	{
//		maFloatRGBA col = m_pScene->GetFogColor();
//		bg_color.Set(col.GetRed() * 255, col.GetGreen() * 255, col.GetBlue() * 255);
//	}
	if (g3dPrefs::CurrentPrefs().m_bRenderMatte)
	{
		bg_color.Set(0,0,0);
	}

	// prepare any targets that the renderer depends on.
	// this is almost like depth first recursion. 
	// the render targets are like child nodes in a tree.
	RenderTargets( i_fSimTime );

	// Adjust the headlight (directional light in direction of camera view)
	g3dLightMgr::EnableHeadlight(g3dPrefs::CurrentPrefs().m_bHeadlightOn);
	g3dLightMgr::SetHeadlightDirection( m_pCamera->GetDirection() );

	// clear
	m_pTarget->Clear( bg_color );
	m_pTarget->BeginScene();

	//	render
    m_NumPrimitivesRendered += m_pRenderer->Render(m_pTarget, *m_pCamera, *m_pScene, i_ViewerLayers, i_fSimTime);

	RenderOverlay();

	//	flip
	m_pTarget->EndScene();
}
