/****************************************************************************\
**	shdwOpacityMapRendererVk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_OPACITYMAPRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwOpacityMapRendererVk.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/G3d/g3dProjectedLight.hpp"
#endif
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_SHDW_PASSOPACITY_HPP
#include "GraphicsVulkan/shdw/shdwPassOpacity.hpp"
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
shdwOpacityMapRendererVk::shdwOpacityMapRendererVk( g3dProjectedLight* pProjLight )
: m_pProjLight( pProjLight )
{
	m_Width = 0;
	m_Height = 0;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwOpacityMapRendererVk::~shdwOpacityMapRendererVk()
{
	ReleaseResources();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwOpacityMapRendererVk::ReleaseResources()
{
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwOpacityMapRendererVk::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{
	G3D_PROFILE_SCOPE("OpacityMapRenderer");

	// Initialize the triangle count
	l_nNumTrianglesRendered = 0;

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(i_Camera);

	// gather scene graph elements into sorted lists
	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();
	g3dSceneRenderUtil::enable_lights();
	m_SceneDatabase.Clear();
	m_SceneDatabase.TraverseLayer(i_fSimTime, layers[0], &i_Camera);

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// simple stupid draw loop
	matRenderTargetTexture* OSM[8] = {NULL};
	for( int i = 0; i < 8; i++ )
	{
		OSM[i] = dynamic_cast<matRenderTargetTexture*>(m_pProjLight->GetOpacityShadowMap(i));
	}

	//	(the light's shadow map is a matShadowMap, so on DX11 too this is
	//	NULL and the deep types draw as the plain ones)
	matRenderTargetTexture* pDepth = NULL;
	if( m_pProjLight->GetHairShadowType() >= HAIR_SHADOW_DOSM4 &&
		m_pProjLight->GetHairShadowType() <= HAIR_SHADOW_DOSM32 )
	{
		pDepth = dynamic_cast<matRenderTargetTexture*>(m_pProjLight->GetShadowMap());
	}
	shdwPassOpacity opacityPass( OSM, &i_Camera, pDepth );
	opacityPass.SetSceneInfo(&m_SceneDatabase);
	opacityPass.SetBounds( m_pProjLight->GetHairMinBound(), m_pProjLight->GetHairMaxBound() );
	opacityPass.SetOpacityVolume( m_pProjLight->GetOpacityVolume() );
	l_nNumTrianglesRendered += opacityPass.Render(i_fSimTime);
	m_pProjLight->SetHairMinBound( opacityPass.m_Near );
	m_pProjLight->SetHairMaxBound( opacityPass.m_Far );

	return l_nNumTrianglesRendered;
}

void shdwOpacityMapRendererVk::InitStates()
{
}

void shdwOpacityMapRendererVk::CleanupStates()
{
}

}
