/****************************************************************************\
**	shdwOpacityMapRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwOpacityMapRendererDX11.hpp"

#include "Core/ma/maPlane.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/Eff/effTexturedData.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/G3d/g3dProjectedLight.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/G3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "GraphicsDX11/shdw/shdwFrameBufferMgr.hpp"
#include "GraphicsDX11/shdw/shdwPassOpacity.hpp"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

namespace
{
	// Stats
	int l_nNumTrianglesRendered = 0;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwOpacityMapRendererDX11::shdwOpacityMapRendererDX11( g3dProjectedLight* pProjLight )
: m_pProjLight( pProjLight )
{
	m_Width = 0;
	m_Height = 0;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwOpacityMapRendererDX11::~shdwOpacityMapRendererDX11()
{
	ReleaseResources();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwOpacityMapRendererDX11::ReleaseResources()
{
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwOpacityMapRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{//PROFILE("Render");
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwOpacityMapRendererDX11::Render" );

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

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	// simple stupid draw loop
	matRenderTargetTexture* OSM[8] = {NULL};
	for( int i = 0; i < 8; i++ )
	{
		OSM[i] = dynamic_cast<matRenderTargetTexture*>(m_pProjLight->GetOpacityShadowMap(i));
	}

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

	D3DPERF_EndEvent();
	return l_nNumTrianglesRendered;
}

void shdwOpacityMapRendererDX11::InitStates()
{
}

void shdwOpacityMapRendererDX11::CleanupStates()
{
}