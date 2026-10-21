/****************************************************************************\
**	shdwDepthRendererVk.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_VELOCITYMAPRENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwVelocityMapRendererVk.hpp"
#endif

#ifndef MA_PLANE_HPP
#include "Core/ma/maPlane.hpp"
#endif
//#include "Graphics/cam/camCamera.hpp"
#ifndef EFF_TEXTUREDDATA_HPP
#include "Graphics/Eff/effTexturedData.hpp"
#endif
#ifndef G2D_WINDOW_HPP
#include "Graphics/g2d/g2dWindow.hpp"
#endif
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/mat/matTextureMgr.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
#endif
#ifndef GVK_G3DDEPTHSTENCILSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dDepthStencilStateMgr.hpp"
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
#ifndef GVK_SHDW_FRAMEBUFFERMGR_HPP
#include "GraphicsVulkan/shdw/shdwFrameBufferMgr.hpp"
#endif
#ifndef GVK_SHDW_PASSDEPTH_HPP
#include "GraphicsVulkan/shdw/shdwPassDepth.hpp"
#endif
#ifndef GVK_SHDW_PASSVELOCITY_HPP
#include "GraphicsVulkan/shdw/shdwPassVelocity.hpp"
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

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_Blend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwVelocityMapRendererVk::shdwVelocityMapRendererVk()
{
	m_Width = 0;
	m_Height = 0;
	m_pWindow = NULL;

	m_pVelocityStateManager = new VelocityStateManager();

	m_lastTime = 0;

	m_pVelocityMat = new matMaterial("VelocityRender.fx");
	m_pVelocityData = dynamic_cast<effTexturedData*>(m_pVelocityMat->GetEffectData());
	DBG_ASSERT(m_pVelocityData, "Could not load shader needed for Velocity rendering");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwVelocityMapRendererVk::~shdwVelocityMapRendererVk()
{
	ReleaseResources();

	delete m_pVelocityMat;

	delete m_pVelocityStateManager;
	m_pVelocityStateManager = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwVelocityMapRendererVk::ReleaseResources()
{
	// force updates on next render call
	m_pWindow = NULL;

	m_pFrameBuffer->ReleaseSurfaces();
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwVelocityMapRendererVk::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
								const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
							   float i_fSimTime )
{//PROFILE("Render");
	G3D_PROFILE_SCOPE("VelocityMapRenderer");

	// Assign shader
	effShaderBaseVk* i_pEffect = (effShaderBaseVk*)matShaderMgr::GetSpecialEffect(("VelocityRender.fx"));
	fxEffect* pEffect = i_pEffect->GetFxEffect();

	// Initialize the triangle count
	l_nNumTrianglesRendered = 0;
	m_pWindow = i_pWindow;

	// Set our global / local frame time value
	g3dSceneGlobal::g_FrameTime = i_fSimTime;

	// make sure temp surfaces are up-to-date
	CreateSurfaces(i_pWindow);

	// Velocity map pass
	shdwPassVelocity velocityPass(m_pFrameBuffer->VelocityBuffer(), m_pVelocityStateManager, &i_Camera, &i_Scene );
	velocityPass.SetSceneInfo(&m_SceneDatabase);
	velocityPass.SetOldTime(m_lastTime);
	l_nNumTrianglesRendered += velocityPass.Render(i_fSimTime);

	// Read in velocity map now stored in m_pVelocityBuffer, and display it!
	i_pWindow->MakeCurrent();
	int w,h;
	i_pWindow->GetDimensions(w,h);

	fxEffectTechnique* pTechnique = pEffect->GetTechniqueByName("DisplayVelocity");

	pEffect->GetVariableByName("velocityBuffer")->AsShaderResource()->SetResource( m_pFrameBuffer->VelocityBuffer()->GetImage() );

	// Render it on a viewport via full-screen quad

	// Motion blur are disabled, these new blend states have never been tested.
	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	fxEffectPass* pPass = pTechnique->GetPassByIndex(0);
	pPass->Apply(0);
	g3dVkUtil::DrawFullScreenQuad( w,h );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

// Motion blur are disabled, these new blend states have never been tested.
	g3dBlendStateMgr::SetBlendState(st_Blend);

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// reset d3d state
	g3dVkUtil::release_textures();


	m_lastTime = i_fSimTime;

	m_pVelocityStateManager->UpdatePrevTransforms();
	velocityPass.ClearObjects();

	return l_nNumTrianglesRendered;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwVelocityMapRendererVk::CreateSurfaces(g2dRenderTarget* i_pWindow)
{
	m_pFrameBuffer->CreateSurfaces(i_pWindow, 0);

	// make render target compatible with window...
	int w,h;
	i_pWindow->GetDimensions(w,h);
	m_Width = w;
	m_Height = h;
}

void shdwVelocityMapRendererVk::DrawNode(const g3dSceneNode* i_pNode)
{
	// resolve material/effect
	const matMaterial* pMaterial = g3dVkUtil::GetMaterial(i_pNode);//m_pAlphaMaskMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

	// Set draw style
	g3dDrawStyleUtilVk::SetDrawStyle(i_pNode->GetDrawStyle());

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_pNode);
	// DrawNode is never used in this class, if it's used in the future the blend state need to be properly set here
	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	l_nNumTrianglesRendered += g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
}

//--------------------------------------------------------------------
//	Sometimes we need to access the pre-buffer pixels that don't get
//	drawn to a window.
//--------------------------------------------------------------------
g2dRenderTarget* shdwVelocityMapRendererVk::GetRawBuffer()
{
	return m_pFrameBuffer->VelocityBuffer();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g2dPFD::PixelFormat shdwVelocityMapRendererVk::GetRawBufferPFD()
{
	return g2dPFD::e_RGBA16f;
}

void shdwVelocityMapRendererVk::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );
	st_Blend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwVelocityMapRendererVk::CleanupStates()
{
	delete st_NoBlend;
	st_NoBlend = NULL;
	delete st_Blend;
	st_Blend = NULL;
	delete ds_Disable_NS;
	ds_Disable_NS = NULL;
	delete ds_Test_Write_LessE_NS;
	ds_Test_Write_LessE_NS = NULL;
}

}
