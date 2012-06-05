/****************************************************************************\
**	pick3dPickBuffer.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Tool/pick3d/pick3dPickBuffer.hpp"

#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "Graphics/g3d/g3dTargetRenderer.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pick3dPickBuffer::pick3dPickBuffer(g3dViewer& i_Viewer)
: m_Viewer(i_Viewer)
{
	m_pPickRenderer = g3dSceneRendererCreate::CreatePickRenderer();

	g2dPFD pfd(g2dPFD::e_RGBA32f, 32*4);
	m_pPickTexture = matTextureMgr::CreateRenderTargetTexture(1,1,false,&pfd,false,true,matTextureMgr::e_Framebuffer);
	m_pPickTarget = m_pPickTexture->GetRenderTargetAPI();

	m_pTargetRenderer = new g3dTargetRenderer(m_pPickTarget, m_pPickRenderer, 
		m_Viewer.GetScene(), &m_PickCamera );
	m_pTargetRenderer->SetBackgroundColor(g2dRGBColor(0,0,0));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
pick3dPickBuffer::~pick3dPickBuffer()
{
	delete m_pTargetRenderer;
	matTextureMgr::ReleaseTexture(m_pPickTexture);
	delete m_pPickRenderer;
}

//--------------------------------------------------------------------
// Render objects at given pixel with color encodings.
//--------------------------------------------------------------------
void pick3dPickBuffer::DoPickRender(int i_X, int i_Y, float i_Time, envType::UInt32 i_PickMask,
									g3dPickInfo& o_PickInfo,
									const std::vector<g3dLayer*>& i_ViewerLayers)
{
	m_PickInfo.Clear();

	int width = 0, height = 0;
	m_Viewer.GetWindow()->GetDimensions(width, height);

	if (i_X >= 0 && i_Y >= 0 && i_X < width && i_Y < height)
	{
		float left = -1.0f + 2.0f * (i_X / float (width));
		float right = -1.0f + 2.0f * (i_X + 1) / float (width);
		float top = -1.0f + 2.0f * (i_Y / float (height));
		float bottom = -1.0f + 2.0f * (i_Y + 1) / float (height);

		// Set up the viewport of the camera to a single pixel
		m_PickCamera = (*m_Viewer.GetCamera()); // Make copy of camera's properties
		m_PickCamera.SetSubViewport(top, bottom, left, right);
		// Must retain aspect ratio of original camera,
		// because I am rendering into a 1x1 pixel area
		m_PickCamera.SetMatchAspectToWindow(false);

		// Set up subviewport in render buffer also
		m_pPickRenderer->SetPickViewport(m_Viewer.GetTarget());

		// Set the filter mask for types of objects to be picked
		m_pPickRenderer->SetPickMask(i_PickMask);

		// Render pick buffer
		m_pTargetRenderer->Render(i_Time, i_ViewerLayers);

		// Restore the old viewport
		m_pPickRenderer->RestoreViewport();

		// Read out the picked object code
		m_pPickRenderer->GetPickInfo(m_pPickTarget, m_PickInfo);
	}

	o_PickInfo = m_PickInfo;
}
