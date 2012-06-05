/*****************************************************************************
**	dcutViewer.cpp
**
**	Viewer for multiple camera views in Cue Form
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/DirectorsCut/Cue/dcutViewer.hpp"

#include "Support/cams/camsFollowUtil.hpp"

#include "Tool/api3d/api3dScene.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Graphics/g2d/g2dSystem.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"

namespace
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
dcutViewer::dcutViewer(g2dSystem &i_System, void* i_Hwnd)
: m_System(i_System), m_CameraIndex(-1)
{
	m_pWindow = i_System.CreateSubWindow(i_Hwnd);
	this->SetWindow(m_pWindow);

	m_pRenderer = g3dSceneRendererCreate::CreateDefaultRenderer();
	this->SetRenderer(m_pRenderer);

	this->SetBackgroundColor( g2dRGBColor(0x00, 0x00, 0x00) );
	this->SetScene(api3dScene::GetScene());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
dcutViewer::~dcutViewer()
{
	m_System.DestroyWindow(m_pWindow);
	delete m_pRenderer;
}


//--------------------------------------------------------------------
// Set which camera to use for this view
//--------------------------------------------------------------------
void dcutViewer::SetCameraIndex(int i_Index)
{
	m_CameraIndex = i_Index;
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
void dcutViewer::Render( float i_fSimTime )
{
	if (m_CameraIndex == -1) return;

	// configures scripted camera
	camCamera* pCamera = camsFollowUtil::ConfigureCamera(m_CameraIndex);
	DBG_ASSERT0(pCamera, "NULL camera from camsFollowUtil");
	this->SetCamera(pCamera);

	int width = 0, height = 0;
	m_pWindow->GetDimensions(width, height);
	pCamera->SetAspect(width, height);

	g3dViewer::Render(i_fSimTime);
}
