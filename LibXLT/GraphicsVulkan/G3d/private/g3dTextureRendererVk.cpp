/****************************************************************************\
**	g3dTextureRendererVk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_G3D_TEXTURERENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dTextureRendererVk.hpp"
#endif

#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G2D_WINDOW_HPP
#include "Graphics/g2d/g2dWindow.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_G3D_RENDERPASS_HPP
#include "GraphicsVulkan/G3d/g3dRenderPass.hpp"
#endif
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif
//#include "GraphicsVk/mat/matRenderTargetTexture.hpp"
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{



namespace
{
	// Stats
	int l_nNumTrianglesRendered = 0;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dTextureRendererVk::g3dTextureRendererVk()
{
	m_Width = 0;
	m_Height = 0;
	m_pWindow = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dTextureRendererVk::~g3dTextureRendererVk()
{
	ReleaseResources();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void g3dTextureRendererVk::ReleaseResources()
{
	// force updates on next render call
	m_pWindow = NULL;
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int g3dTextureRendererVk::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{//PROFILE("Render");
	G3D_PROFILE_SCOPE("TextureRenderer");

	// Initialize the triangle count
	l_nNumTrianglesRendered = 0;

	m_pWindow = i_pWindow;

	int w, h;
	i_pWindow->GetDimensions(w, h);
	if (m_Width != w || m_Height != h)
	{
		m_Width = w;
		m_Height = h;
	}

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	i_pWindow->MakeCurrent();

	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	// gather scene graph elements into sorted lists
	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();
	g3dVkUtil::SetOverrideMaterial(NULL);

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_FRONT_BIT, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	for (int i = 0; i < layers.size(); i++)
	{
		g3dRenderLayer rLayer;
		rLayer.Set(layers[i], &i_Camera, i_pWindow);
		rLayer.PerFrameInit(i_fSimTime);
		l_nNumTrianglesRendered += rLayer.Render(i_fSimTime);
	}

	// Also add in the extra layers that are specific to this viewer
	for (int i = 0; i < i_ViewerLayers.size(); i++)
	{
		g3dRenderLayer rLayer;
		rLayer.Set(i_ViewerLayers[i], &i_Camera, i_pWindow);
		rLayer.PerFrameInit(i_fSimTime);
		l_nNumTrianglesRendered += rLayer.Render(i_fSimTime);
	}

	// reset d3d state
	g3dVkUtil::release_textures();

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

	// Display debug information for number of triangles
//	char num[64];
//	sprintf(num, "tris: %d", l_nNumTriangles);
//	i_pWindow->SetDebugInfo(2, num);

	return l_nNumTrianglesRendered;
}

}
