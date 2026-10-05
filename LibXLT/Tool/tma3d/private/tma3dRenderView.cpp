/****************************************************************************\
**	tma3dRenderView.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Tool/tma3d/tma3dRenderView.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/gpx/gpxCamera.hpp"
#include "Tool/pick3d/pick3dPickBuffer.hpp"
#include "Tool/tma3d/tma3dViewerMgr.hpp"

#include <math.h>


//============================================================================
// Static variables
//============================================================================
tma3dRenderView* tma3dRenderView::sm_pActiveRenderView = NULL;

//--------------------------------------------------------------------
// i_Window should have been created by a g2dSystem, usually with
//	CreateSubWindow. Since the system maintains ownership of the
//	window, it will not be deleted by this class in the destructor.
//
// A g3dViewer will be created in this constructor if the
//	i_pViewer argument is NULL. Otherwise, it will take 
//	ownership of the passed in viewer.
//
// Three renderers will be created, you can control which style
//	of renderer is used with the SetRenderer function.
//--------------------------------------------------------------------
tma3dRenderView::tma3dRenderView(g2dWindow* i_pWindow,
								 g3dViewer *i_pViewer, 
								 const std::vector<g3dLayer*>& i_ViewerLayers)
:	m_pWindow(i_pWindow),
	m_bViewerEnabled(false),
	m_pCameraProxy(NULL),
	m_ViewerLayers(i_ViewerLayers)
{
	DBG_ASSERT(i_pWindow != NULL, "tma3dRenderView, Window pointer is NULL.");

	// Create renderers
	m_Renderers.resize(e_NumTypes);

	m_Renderers[e_Default] = g3dSceneRendererCreate::CreateDefaultRenderer();

	m_Renderers[e_HDR] = g3dSceneRendererCreate::CreateHDRRenderer();
	if (m_Renderers[e_Default])
	m_Renderers[e_Default]->ShareBuffers(m_Renderers[e_HDR]);

	m_Renderers[e_AmbientOcclusion] = g3dSceneRendererCreate::CreateAmbientOcclusionRenderer();
	if (m_Renderers[e_Default])
	m_Renderers[e_Default]->ShareBuffers(m_Renderers[e_AmbientOcclusion]);

	m_Renderers[e_Depth] = g3dSceneRendererCreate::CreateDepthRenderer();
	if (m_Renderers[e_Default])
	m_Renderers[e_Default]->ShareBuffers(m_Renderers[e_Depth]);

	m_Renderers[e_ShadowMask] = g3dSceneRendererCreate::CreateShadowMaskRenderer();
	if (m_Renderers[e_Default])
	m_Renderers[e_Default]->ShareBuffers(m_Renderers[e_ShadowMask]);

	m_Renderers[e_Normals] = g3dSceneRendererCreate::CreateNormalRenderer();
	if (m_Renderers[e_Default])
	m_Renderers[e_Default]->ShareBuffers(m_Renderers[e_Normals]);

	m_Renderers[e_IlluminationOnly] = g3dSceneRendererCreate::CreateIlluminationRenderer();
	if (m_Renderers[e_Default])
	m_Renderers[e_Default]->ShareBuffers(m_Renderers[e_IlluminationOnly]);

	m_Renderers[e_Materials] = g3dSceneRendererCreate::CreateMaterialsRenderer();
	if (m_Renderers[e_Default])
	m_Renderers[e_Default]->ShareBuffers(m_Renderers[e_Materials]);

	m_Renderers[e_ReflectionOnly] = g3dSceneRendererCreate::CreateReflectionOnlyRenderer();
	if (m_Renderers[e_Default])
	m_Renderers[e_Default]->ShareBuffers(m_Renderers[e_ReflectionOnly]);

	m_Renderers[e_GlobalIllumination] = g3dSceneRendererCreate::CreateGlobalIlluminationRenderer();
	if (m_Renderers[e_Default])
	m_Renderers[e_Default]->ShareBuffers(m_Renderers[e_GlobalIllumination]);

	m_Renderers[e_Glow] = g3dSceneRendererCreate::CreateGlowRenderer();
	if (m_Renderers[e_Default])
	m_Renderers[e_Default]->ShareBuffers(m_Renderers[e_Glow]);

	m_Renderers[e_VelocityMap] = g3dSceneRendererCreate::CreateVelocityMapRenderer();
	if (m_Renderers[e_Default])
	m_Renderers[e_Default]->ShareBuffers(m_Renderers[e_VelocityMap]);

	m_CurrentRenderer = e_Default;
	if (i_pViewer)
	{
		m_pViewer = i_pViewer;
		m_pViewer->SetWindow( m_pWindow );
		m_pViewer->SetRenderer( m_Renderers[m_CurrentRenderer] );
	}
	else
	{
		m_pViewer = new g3dViewer(m_pWindow, m_Renderers[m_CurrentRenderer]);
	}
	//m_pViewer->SetPrefs(&m_RenderPrefs);

	// automatically set apect ratio of camera based on window size
	//m_pViewer->SetMatchAspectToWindow(true);  // camera determines this now

	//m_pViewer->SetBackgroundColor( g2dRGBColor(0x30, 0x80, 0xa0) );
	m_pViewer->SetBackgroundColor( g2dRGBColor(0x00, 0x00, 0x00) );

	// Set defaults for camera and scene using Tool3d library
	m_pViewer->SetCamera(&cam3dMgr::GetCamera());
	m_pViewer->SetScene(api3dScene::GetScene());

	// Enable viewer be default
	this->EnableViewer( true );

	// Set up pick buffer for our viewer
	m_pPickBuffer = new pick3dPickBuffer(*m_pViewer);
}

//--------------------------------------------------------------------
// Deletes the renderer and viewer owned by this class.
//--------------------------------------------------------------------
tma3dRenderView::~tma3dRenderView()
{
	delete m_pPickBuffer;

	this->EnableViewer( false );
	delete m_pViewer;

	envSTLHelpers::DeleteContainer(m_Renderers);

	// window is owned by system, do not delete here
}

//--------------------------------------------------------------------
// Call this to enable or disable rendering of this viewer.
// This is usually hooked up to the Enabled flag of the 
//	rendering panel.
//--------------------------------------------------------------------
void tma3dRenderView::EnableViewer(bool i_bEnable)
{
	if (i_bEnable && !m_bViewerEnabled)
	{
		tma3dViewerMgr::AddViewer( m_pViewer );
	}
	else if (!i_bEnable && m_bViewerEnabled)
	{
		tma3dViewerMgr::RemoveViewer( m_pViewer );
	}
	m_bViewerEnabled = i_bEnable;
}

//--------------------------------------------------------------------
// Set Camera for viewer to use
//--------------------------------------------------------------------
void tma3dRenderView::SetCameraProxy(gpxCamera* i_pCameraProxy)
{
	m_pCameraProxy = i_pCameraProxy;
	m_pViewer->SetCamera( &i_pCameraProxy->GetCamera() );
}
//void tma3dRenderView::SetCamera(camCamera* i_pCamera)
//{
//	m_pViewer->SetCamera( i_pCamera );
//}

//--------------------------------------------------------------------
// Set Camera into cam3dMgr in order to activate the camera 
//	manipulator for our camera.
//--------------------------------------------------------------------
void tma3dRenderView::ActivateCameraManipulator()
{
	cam3dMgr::EnableManip(true);

	camCamera *pCamera = m_pViewer->GetCamera();
	if (pCamera == &cam3dMgr::GetEditorCamera())
	{
		cam3dMgr::EndScriptedCamera();
	}
	else if (pCamera->IsOrthographic())
	{
		cam3dMgr::UseScriptedCamera(m_pCameraProxy, cam3dMgr::e_OrthoPan);
	}
	else
	{
		cam3dMgr::UseScriptedCamera(m_pCameraProxy);
	}
}
//--------------------------------------------------------------------
// Get the depth based on the camera target position
//--------------------------------------------------------------------
float tma3dRenderView::GetCameraDepth()
{
	camCamera *pCamera = m_pViewer->GetCamera();
	return (pCamera->GetPosition().m_Z - pCamera->GetTarget().m_Z);
}


//--------------------------------------------------------------------
// Set style of renderer using enumeration
//--------------------------------------------------------------------
void tma3dRenderView::SetRenderer(RendererType i_Type, 
								  g3dPrefs::g3dRenderPrefs* i_pPrefs /*= NULL*/)
{
	DBG_ASSERT(i_Type>=0 && i_Type<m_Renderers.size(), "RendererType out of range " <<  i_Type << " < " << m_Renderers.size());

	if (i_pPrefs)
	{
		// copy prefs here
		m_RenderPrefs = *i_pPrefs;
	}

	m_CurrentRenderer = i_Type;
	m_pViewer->SetRenderer(m_Renderers[m_CurrentRenderer]);
}

//--------------------------------------------------------------------
// Resize back buffer of window
//--------------------------------------------------------------------
void tma3dRenderView::ResizeWindow(int i_Width, int i_Height, int iWidthDIP, int iHeightDIP)
{
	if (i_Width > 0 && i_Height > 0)
	{
		//DBG_LOG2("Setting render window size: %d %d", i_Width, i_Height);

		// render at the window size
		m_pWindow->SetRenderResolution(0, 0);
		m_pWindow->ResizeWindow((i_Width), (i_Height));

        if (iWidthDIP == 0) {
            iWidthDIP = i_Width;
        }
        if (iHeightDIP == 0) {
            iHeightDIP = i_Height;
        }
        // Setting virtual resolution will keep text the same size 
		// when the window resizes and will avoid stretching when 
		// aspect ratio changes
        m_pWindow->SetVirtualResolution(iWidthDIP, iHeightDIP);
	}
}

//--------------------------------------------------------------------
// Resize back buffer of window to the panel size, render at the
//	given render size, and scale the image to fit the panel.
//--------------------------------------------------------------------
void tma3dRenderView::ResizeWindowToFit(int i_PanelWidth, int i_PanelHeight,
										int i_RenderWidth, int i_RenderHeight,
										int i_RenderWidthDIP, int i_RenderHeightDIP)
{
	if (i_PanelWidth > 0 && i_PanelHeight > 0 && i_RenderWidth > 0 && i_RenderHeight > 0)
	{
		m_pWindow->SetRenderResolution(i_RenderWidth, i_RenderHeight);
		m_pWindow->ResizeWindow(i_PanelWidth, i_PanelHeight);

		if (i_RenderWidthDIP <= 0 || i_RenderHeightDIP <= 0)
		{
			i_RenderWidthDIP = i_RenderWidth;
			i_RenderHeightDIP = i_RenderHeight;
		}
		// Text is laid out in the rendered image, so base the virtual
		// resolution on the render size
		m_pWindow->SetVirtualResolution(i_RenderWidthDIP, i_RenderHeightDIP);
	}
}

//--------------------------------------------------------------------
// Convert a position in panel pixels to a pixel in the rendered
//	image. The result is outside the image in the black bars.
//--------------------------------------------------------------------
void tma3dRenderView::PanelToRenderPixel(int& io_X, int& io_Y) const
{
	int x, y, width, height;
	m_pWindow->GetPresentRect(x, y, width, height);
	int render_width, render_height;
	m_pWindow->GetDimensions(render_width, render_height);

	if (width > 0 && height > 0)
	{
		// map pixel centers so a 1:1 rect leaves positions unchanged
		io_X = (int)floor((io_X - x + 0.5) * render_width / width);
		io_Y = (int)floor((io_Y - y + 0.5) * render_height / height);
	}
}

//--------------------------------------------------------------------
// Size of the rendered image in pixels
//--------------------------------------------------------------------
void tma3dRenderView::GetRenderSize(int& o_Width, int& o_Height) const
{
	m_pWindow->GetDimensions(o_Width, o_Height);
}

//--------------------------------------------------------------------
// Render objects at given pixel with color encodings.
// Returns pick code that can be used to search for the picked
// g3dSceneNode in the scene graph. A returned value of 0
// means nothing was picked.
//--------------------------------------------------------------------
void tma3dRenderView::DoPickRender(int i_X, int i_Y, float i_Time, g3dPickInfo& o_PickInfo,
						envType::UInt32 i_PickMask)
{
	return m_pPickBuffer->DoPickRender(i_X, i_Y, i_Time, i_PickMask, o_PickInfo, m_ViewerLayers);
}

//--------------------------------------------------------------------
// Only one render view can have the focus at a time. 
//--------------------------------------------------------------------
//static 
void tma3dRenderView::SetActiveRenderView(tma3dRenderView *i_pRenderView)
{
	sm_pActiveRenderView = i_pRenderView;
}
//static 
tma3dRenderView* tma3dRenderView::GetActiveRenderView()
{
	return sm_pActiveRenderView;
}
