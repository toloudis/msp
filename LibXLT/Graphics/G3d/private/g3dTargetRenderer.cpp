/****************************************************************************\
**	g3dTargetRenderer.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dTargetRenderer.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/g2d/g2dRenderTarget.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"


//----------------------------------------------------------------------------
// The root node is not owned by the layer, just pointed to
//----------------------------------------------------------------------------
g3dTargetRenderer::g3dTargetRenderer()
:	m_pTarget(NULL), 
	m_pRenderer(NULL), 
	m_pScene(NULL), 
	m_pCamera(NULL),
	m_fAspectRatio(1.0),
//	m_bMatchAspectToWindow(false),
	m_NumPrimitivesRendered(0),
	m_pPrefs(NULL)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dTargetRenderer::g3dTargetRenderer(g2dRenderTarget* i_pTarget, g3dSceneRenderer* i_pRenderer)
:	m_pTarget(i_pTarget), 
	m_pRenderer(i_pRenderer),
	m_pScene(NULL), 
	m_pCamera(NULL),
	m_fAspectRatio(1.0),
//	m_bMatchAspectToWindow(false),
	m_NumPrimitivesRendered(0),
	m_pPrefs(NULL)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dTargetRenderer::g3dTargetRenderer(g2dRenderTarget* i_pTarget, g3dSceneRenderer* i_pRenderer,
									 g3dScene* i_pScene, camCamera* i_pCamera)
:	m_pTarget(i_pTarget), 
	m_pRenderer(i_pRenderer),
	m_pScene(i_pScene), 
	m_pCamera(i_pCamera),
	m_fAspectRatio(1.0),
//	m_bMatchAspectToWindow(false),
	m_NumPrimitivesRendered(0),
	m_pPrefs(NULL)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dTargetRenderer::~g3dTargetRenderer()
{
}

//----------------------------------------------------------------------------
// The window is not owned by the viewer, just pointed to
//----------------------------------------------------------------------------
g2dRenderTarget* g3dTargetRenderer::GetTarget() const
{
	return m_pTarget;
}
void g3dTargetRenderer::SetTarget(g2dRenderTarget* i_pTarget)
{
	m_pTarget = i_pTarget;
}

//----------------------------------------------------------------------------
// The renderer is not owned by the viewer, just pointed to
//----------------------------------------------------------------------------
g3dSceneRenderer* g3dTargetRenderer::GetRenderer() const
{
	return m_pRenderer;
}
void g3dTargetRenderer::SetRenderer(g3dSceneRenderer* i_pRenderer)
{
//	if (m_pRenderer != NULL)
//		m_pRenderer->ReleaseResources();

	m_pRenderer = i_pRenderer;
}

//----------------------------------------------------------------------------
// The scene is not owned by the viewer, just pointed to
//----------------------------------------------------------------------------
g3dScene* g3dTargetRenderer::GetScene() const
{
	return m_pScene;
}
void g3dTargetRenderer::SetScene(g3dScene* i_pScene)
{
	m_pScene = i_pScene;
}

//----------------------------------------------------------------------------
// The camera is not owned by the viewer, just pointed to
//----------------------------------------------------------------------------
camCamera* g3dTargetRenderer::GetCamera() const
{
	return m_pCamera;
}

void g3dTargetRenderer::SetCamera(camCamera* i_pCamera)
{
	m_pCamera = i_pCamera;
}

//----------------------------------------------------------------------------
// Gets the aspect ratio that is set by cptrModeRender.cpp
//----------------------------------------------------------------------------
void g3dTargetRenderer::SetAspectRatio(int PixelAspectRatio)
{
	switch(PixelAspectRatio)
	{
		case 0:
			m_fAspectRatio = 1.0f;
			break;
		case 1:
			m_fAspectRatio = 1.06667f;
			break;
		case 2:
			m_fAspectRatio = 0.9f;
			break;
		default:
			m_fAspectRatio = 1.0f;
			break;
	}
}
	
float g3dTargetRenderer::GetAspectRatio()
{
	return m_fAspectRatio;
}

//----------------------------------------------------------------------------
// If this flag is set to true, then the aspect ratio of the camera
//	will be set to match the window's width and height.
//----------------------------------------------------------------------------
//** Note that this behavior is now controlled by the camera.**
//bool g3dTargetRenderer::GetMatchAspectToWindow() const
//{
//	return m_bMatchAspectToWindow;
//}
//void g3dTargetRenderer::SetMatchAspectToWindow(bool i_pVal)
//{
//	m_bMatchAspectToWindow = i_pVal;
//}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
void g3dTargetRenderer::Render( float i_fSimTime, bool i_bClear )
{
	std::vector<g3dLayer*> empty_layers;
	this->Render(i_fSimTime, empty_layers, i_bClear);

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void g3dTargetRenderer::Render( float i_fSimTime, const std::vector<g3dLayer*>& i_ViewerLayers, bool i_bClear )
{
	if (!m_pRenderer->IsEnabled())
		return;

	m_NumPrimitivesRendered = 0;

	g2dRGBColor bg_color = m_BackgroundColor;

	// if fog is enabled, set background color to fog's color
	if (m_pScene->IsFogEnabled())
	{
		maFloatRGBA col = m_pScene->GetFogColor();
		bg_color.Set(col.GetRed() * 255, col.GetGreen() * 255, col.GetBlue() * 255);
	}
	if (g3dPrefs::CurrentPrefs().m_bRenderMatte)
	{
		bg_color.Set(0,0,0);
	}

	// If requested, set the aspect ratio of the camera to match the
	// width/height of the window we are rendering into
	//if ( m_bMatchAspectToWindow )
	if ( m_pCamera->GetMatchAspectToWindow() )
	{
		int width = 0, height = 0;
		m_pTarget->GetDimensions(width, height);
		if (height > 0)
		{
			float aspect = width / (float)height;
			m_pCamera->SetAspect(aspect);
		}
	}
	float originalAspect = m_pCamera->GetAspect();
	m_pCamera->SetAspect(m_pCamera->GetAspect() * GetAspectRatio() );

	//bga - Since planar reflections need the aspect ratio 
	// of the render target, the RenderTargets() call needs to be after the 
	// apect ratio is full set up.
	//
	// prepare any targets that the renderer depends on.
	// this is almost like depth first recursion. 
	// the render targets are like child nodes in a tree.
	RenderTargets( i_fSimTime );

	// Adjust the headlight (directional light in direction of camera view)
	g3dLightMgr::EnableHeadlight(g3dPrefs::CurrentPrefs().m_bHeadlightOn);
	g3dLightMgr::SetHeadlightDirection( m_pCamera->GetDirection() );
	
	if( i_bClear )
	{
		// clear
		m_pTarget->Clear( bg_color );
		m_pTarget->BeginScene();
	}

	// set up rendering prefs
	g3dPrefs::g3dRenderPrefs* originalPrefs = &g3dPrefs::CurrentPrefs();
	if (m_pPrefs)
	{
		g3dPrefs::SetPrefs(m_pPrefs);
	}

	//	render scene plus additional viewer-only layers
	m_NumPrimitivesRendered += m_pRenderer->Render(m_pTarget, *m_pCamera, 
												   *m_pScene, i_ViewerLayers, i_fSimTime);

	// restore rendering prefs
	if (m_pPrefs)
	{
		g3dPrefs::SetPrefs(originalPrefs);
	}

	m_pCamera->SetAspect(originalAspect);
	RenderOverlay();

	//	flip
	m_pTarget->EndScene();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void g3dTargetRenderer::Present()
{
}

//----------------------------------------------------------------------------
// Returns number of primitives rendered in last rendered frame
//----------------------------------------------------------------------------
int g3dTargetRenderer::GetNumPrimitivesRendered()
{
	return m_NumPrimitivesRendered;
}

//----------------------------------------------------------------------------
// Set background color for window
//----------------------------------------------------------------------------
const g2dRGBColor& g3dTargetRenderer::GetBackgroundColor() const
{
	return m_BackgroundColor;
}
void g3dTargetRenderer::SetBackgroundColor(const g2dRGBColor& i_Color)
{
	m_BackgroundColor = i_Color;
}

//----------------------------------------------------------------------------
// Set render preferences for this viewer's renders 
//----------------------------------------------------------------------------
void g3dTargetRenderer::SetPrefs(g3dPrefs::g3dRenderPrefs* i_pRenderPrefs)
{
	m_pPrefs = i_pRenderPrefs;
}

