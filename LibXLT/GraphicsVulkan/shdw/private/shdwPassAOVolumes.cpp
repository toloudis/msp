/****************************************************************************\
**	shdwPassAOVolumes.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSAOVOLUMES_HPP
#include "GraphicsVulkan/shdw/shdwPassAOVolumes.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

#ifndef MA_CONSTANTS_HPP
#include "Core/ma/maConstants.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/g2d/g2dRenderTarget.hpp"
#endif
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/mat/matTextureMgr.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef GVK_BUMP_BUMPRENDERER_HPP
#include "GraphicsVulkan/bump/bumpBumpRenderer.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
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
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef GVK_G3DLIGHTMGRVK_HPP
#include "GraphicsVulkan/G3d/g3dLightMgrVk.hpp"
#endif
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif
#ifndef GVK_G3D_TRANSPARENCYSORTVK_HPP
#include "GraphicsVulkan/G3d/g3dTransparencySortVk.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_SHDW_PASSNORMALS_HPP
#include "GraphicsVulkan/shdw/shdwPassNormals.hpp"
#endif
#ifndef GVK_SHDW_PASSZFILL_HPP
#include "GraphicsVulkan/shdw/shdwPassZFill.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

#include <algorithm>
#include <cmath>

namespace gvk
{

namespace
{
	matMaterial l_AOMat;
	matMaterial l_DrawHairMat;
	matMaterial l_DepthMat;

	g3dBlendStateMgr::BlendState* stp_SubSatAlpha = NULL;
	g3dBlendStateMgr::BlendState* stp_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_NoBlendDepthOnly = NULL;
	g3dBlendStateMgr::BlendState* st_AOBlend = NULL;
	g3dBlendStateMgr::BlendState* st_AOBlendRestore = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* dsp_Test_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* dsp_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_Replace = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_Ref_Replace = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_LessE_Ref_Equal = NULL;

	matTexture* l_pACosTexture = NULL;

	const VkColorComponentFlags c_WriteRGB = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
	const VkColorComponentFlags c_WriteAll = c_WriteRGB | VK_COLOR_COMPONENT_A_BIT;
}; // namespace

void shdwPassAOVolumes::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP,
		VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP,
		VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_REPLACE( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP,
		VK_STENCIL_OP_REPLACE, VK_COMPARE_OP_ALWAYS );
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_EQUAL( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP,
		VK_STENCIL_OP_REPLACE, VK_COMPARE_OP_EQUAL );

	stp_SubSatAlpha = new g3dBlendStateMgr::BlendState(false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		c_WriteRGB);

	stp_NoBlend = new g3dBlendStateMgr::BlendState(false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		c_WriteAll);;
	st_NoBlendDepthOnly = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		0);

	st_AOBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_COLOR,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_OP_ADD,
		c_WriteRGB );
	st_AOBlendRestore = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_COLOR,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ZERO,VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_OP_ADD,
		c_WriteAll);

	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState(
		false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, /*0,*/ 0xff, 0xff, OP_KEEP, OP_KEEP );

	ds_Test_Write_LessE_Replace = new g3dDepthStencilStateMgr::DepthStencilState(
		true, true, VK_COMPARE_OP_LESS_OR_EQUAL, true, /*0,*/ 0xff, 0xff, OP_REPLACE, OP_KEEP );
	ds_Test_Write_LessE_Ref_Replace = new g3dDepthStencilStateMgr::DepthStencilState(
		true, true, VK_COMPARE_OP_LESS_OR_EQUAL, true, /*1,*/ 0xff, 0xff, OP_REPLACE, OP_KEEP );

	ds_Test_LessE_Ref_Equal = new g3dDepthStencilStateMgr::DepthStencilState(
		true, false, VK_COMPARE_OP_LESS_OR_EQUAL, true, /*1,*/ 0xff, 0xff, OP_EQUAL, OP_KEEP );

	dsp_Test_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState(
		true, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT);
	dsp_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState(
		true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT);

	g2dPFD pfd(g2dPFD::e_Float32, 32);
	const int acosTexSize = 512;
	float acosdata[acosTexSize];
	for (int i = 0; i < acosTexSize; i++)
	{
		// 0-->acos(-1),  511-->acos(1)
		acosdata[i] = acosf(((i/511.0f)-0.5f)*2.0f);// * maConstants::c_fPI_Div_2;
	}
	l_pACosTexture = matTextureMgr::CreateTexture(acosTexSize, 1,
		&pfd,
		false,
		acosdata);
}

void shdwPassAOVolumes::CleanupStates()
{
	matTextureMgr::ReleaseTexture(l_pACosTexture);
	l_pACosTexture = NULL;

	delete stp_SubSatAlpha;					stp_SubSatAlpha = NULL;
	delete stp_NoBlend;						stp_NoBlend = NULL;
	delete st_NoBlendDepthOnly;				st_NoBlendDepthOnly = NULL;
	delete st_AOBlend;						st_AOBlend = NULL;
	delete st_AOBlendRestore;				st_AOBlendRestore = NULL;
	delete dsp_Test_Write_LessE_NS;			dsp_Test_Write_LessE_NS = NULL;
	delete dsp_Test_LessE_NS;				dsp_Test_LessE_NS = NULL;
	delete ds_Test_Write_LessE_Replace;		ds_Test_Write_LessE_Replace = NULL;
	delete ds_Test_Write_LessE_Ref_Replace;	ds_Test_Write_LessE_Ref_Replace = NULL;
	delete ds_Test_LessE_Ref_Equal;			ds_Test_LessE_Ref_Equal = NULL;
	delete ds_Disable_NS;					ds_Disable_NS = NULL;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassAOVolumes::shdwPassAOVolumes()
	: m_SceneInfo(NULL), m_pCamera(NULL)
{
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassAOVolumes::shdwPassAOVolumes(g2dRenderTarget* i_pFinalTarget, matRenderTargetTexture* i_pAO,
									 matRenderTargetTexture* i_pPositions,
									 matRenderTargetTexture* i_pNormals,
									 matRenderTargetTexture* i_pDepth,
									 const camCamera* i_pCamera,
									 const ssaoParams& i_Params)
:	m_SceneInfo(NULL), m_pCamera(i_pCamera),
	m_PositionBuffer(i_pPositions), m_NormalBuffer(i_pNormals),
	m_AOBuffer(i_pAO),
	m_DepthBuffer(i_pDepth),
	m_Params(i_Params)
{
	m_pRenderTarget = i_pFinalTarget;

	InitializeMaterial();
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassAOVolumes::~shdwPassAOVolumes()
{
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassAOVolumes::InitializeMaterial()
{
	// lazy init so that this material can be reused across instantiations.
	if (!l_AOMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(std::string("Special/AOVolumes.fx"), matShaderMgr::GetSpecialEffect("AOVolumes.fx"));
		l_AOMat.SetShaderParams(p);
	}

//	if (!l_DrawHairMat.GetShaderParams())
//	{
//		shared_ptr<effShaderParams> p(new effShaderParams());
//		p->SetShaderName(itString("Special/HairDefault.fx"), matShaderMgr::GetSpecialEffect("HairDefault.fx"));
//		l_DrawHairMat.SetShaderParams(p);
//	}

	// lazy init so that this material can be reused across instantiations.
	if (!l_DepthMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(std::string("Special/DepthRender.fx"), matShaderMgr::GetSpecialEffect("DepthRender.fx"));
		l_DepthMat.SetShaderParams(p);
	}
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
int shdwPassAOVolumes::Render(float i_time)
{
	G3D_PROFILE_SCOPE("PassAOVolumes");
	camPassBuffersData passBuffersData;
	m_pCamera->GetPassBuffersParams(passBuffersData);
	float top, bottom, left, right;
	m_pCamera->GetSubViewport(top, bottom, left, right);
	if ( passBuffersData.m_AOBuffer )
	{
		g3dVkUtil::BlendBuffers(m_pRenderTarget,passBuffersData.m_AOBuffer,passBuffersData.m_AOBlendOp, passBuffersData.m_AOIntensity,
								  top, bottom, left, right);
		return 0;
	}

	m_stats.Reset();

	if (m_DepthBuffer && g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		g2dVkGlobal::SetColorTarget( NULL );
		m_DepthBuffer->MakeDepthCurrent();
	}

	// 1. render eye-space normals into buffer. FULL RES
	shdwPassNormals normalPass(m_NormalBuffer, m_pCamera);
	normalPass.SetSceneInfo(m_SceneInfo);
	m_stats.m_nTriangles += normalPass.Render(i_time);

	// 2. render eye-space positions into buffer. FULL RES
	shdwPassNormals posPass(m_PositionBuffer, m_pCamera);
	posPass.SetRenderPositions(true);
	posPass.SetSceneInfo(m_SceneInfo);
	m_stats.m_nTriangles += posPass.Render(i_time);

	// prepare zbuffer for AO pass
	m_AOBuffer->MakeCurrent();
	// clear depth too
	m_AOBuffer->Clear(maFloatRGBA(0,0,0,1), true);
//	m_pRenderTarget->Clear(maFloatRGBA(1,1,1,1));

	// render target resolution
	int w=1,h=1;
	m_pRenderTarget->GetDimensions(w,h);

	// reduce the AO framebuffer resolution.
	w = w >> g3dPrefs::CurrentPrefs().m_AOResolutionReduce;
	h = h >> g3dPrefs::CurrentPrefs().m_AOResolutionReduce;
	w = std::max(w,1);
	h = std::max(h,1);
	vkdViewport vprt;
	vprt.m_X = vprt.m_Y = 0;
	vprt.m_Width = (float)w;
	vprt.m_Height = (float)h;
	vprt.m_MinDepth = 0;
	vprt.m_MaxDepth = 1;
	vkdGlobal::Context().SetViewports(1, &vprt);

	shdwPassZFill depthPrePass;
	depthPrePass.SetSceneInfo(m_SceneInfo);
	m_stats.m_nTriangles += depthPrePass.Render(i_time);

	// retain depth buffer?
//	m_pRenderTarget->ClearDepthStencil(1.0f, g2dVkGlobal::HasStencil(), 0);

	//use subtractive, saturating alpha blending
	g3dBlendStateMgr::SetBlendState(stp_SubSatAlpha);

	// Z Buffering
	// disable depth write, enable depth test.
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(),
		g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// setup effect params that apply to all objects
	effShaderBaseVk* pEffect = (effShaderBaseVk*)matShaderMgr::GetEffect(l_AOMat);
	pEffect->SetTechnique("Default");
	fxEffect* pD3DXEffect = pEffect->GetFxEffect();

	// acos lookup texture
	vkdImage* acosView = g3dVkTextureUtil::GetImage(l_pACosTexture);
	pD3DXEffect->GetVariableByName("g_ACos")->AsShaderResource()->SetResource(acosView);

	// distance cutoff
	pD3DXEffect->GetVariableByName("g_d")->AsScalar()->SetFloat( m_Params.m_AORadius );
	pD3DXEffect->GetVariableByName("g_Attenuation")->AsScalar()->SetFloat( m_Params.m_Attenuation );

	// source textures
	vkdImage* posView = g3dVkTextureUtil::GetImage(m_PositionBuffer);
	pD3DXEffect->GetVariableByName("g_Positions")->AsShaderResource()->SetResource(posView);
	vkdImage* norView = g3dVkTextureUtil::GetImage(m_NormalBuffer);
	pD3DXEffect->GetVariableByName("g_Normals")->AsShaderResource()->SetResource(norView);
//	vkdGlobal::Context().SetShaderResources(0, 1, &aRes);

	float targetRes[4] = {(float)w,(float)h,1.0f/(float)w,1.0f/(float)h};
	pD3DXEffect->GetVariableByName("g_AOTargetRes")->AsVector()->SetFloatVector(targetRes);

	pD3DXEffect->GetVariableByName("g_ClipPlaneEpsilon")->AsScalar()->SetFloat( m_Params.m_ClipPlaneEpsilon );
	pD3DXEffect->GetVariableByName("g_NoClipPlaneEpsilon")->AsScalar()->SetFloat( m_Params.m_NoClipPlaneEpsilon );
	pD3DXEffect->GetVariableByName("g_AreaRatioEpsilon")->AsScalar()->SetFloat( m_Params.m_AreaRatio );
	pD3DXEffect->GetVariableByName("g_BehindPlaneEpsilon")->AsScalar()->SetFloat( m_Params.m_BehindPlaneEpsilon );

	int i,n;

	// since AO Volumes are expensive, limit the number of indices per draw call.
	static const int nIndicesPerDrawCall = 1500;
	bumpBumpRenderer::SetDrawLimit(nIndicesPerDrawCall);
	// BE SURE TO RESTORE WHEN DONE.

	const nodeCacheList& nonShadowNodes = m_SceneInfo->GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}
	const nodeCacheList& shadowNodes = m_SceneInfo->GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}
	const nodeCacheList& nonSolidNodes = m_SceneInfo->GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}
	//	(DX11's #if 0 block for transparent nodes is left out)

	//hair nodes
#ifdef HAIR_SUPPORTED
	if (g3dPrefs::CurrentPrefs().m_bEnableHair)
	{
		g3dTransparencySortVk* pHairNodes = m_SceneInfo->GetHairNodes();
		const TranspNodeVector& tHNodes = pHairNodes->GetTransparentNodes();
		TranspNodeVector::const_iterator it, end = tHNodes.end();

		for (it = tHNodes.begin(); it != end; ++it)
		{
			sNodePlusState NS;
			NS.m_pNode = it->m_pSceneNode;
			NS.m_StateCache = it->m_RenderStateCache;

//			RenderHairNode( NS );

		}
	}
#endif//HAIR_SUPPORTED

	// done with drawing AO volumes.
	// restore draw limit
	bumpBumpRenderer::SetDrawLimit(-1);

	vkdImage* nullPSR[3] = {NULL, NULL, NULL};
	vkdGlobal::Context().SetShaderResources(0,3,nullPSR);

	//	(DX11 unbinds the geometry shader here; every Apply sets all the
	//	stages on Vulkan)

	// debugging:
//	matTextureMgr::SaveTextureToFile(m_AOBuffer, itString("TESTAOBUFFER.tif"));

	// mask out objects that are not to receive occlusion.
	// stenciling could also be done earlier, when drawing normals and positions to buffer.
	DrawStencilPass(m_pRenderTarget);

	if (g3dPrefs::CurrentPrefs().m_bEnableSSAOBlur)
	{
		BlurAO(m_pRenderTarget);
	}
	else
	{
		BlendAO(m_pRenderTarget, m_AOBuffer);
	}

	// clear the mask.
	ClearStencil(m_pRenderTarget);

	// restore some state.
	g3dDrawStyleUtilVk::RestoreNormalDrawStyle();

	// restore blend stae, can be drop after reorg
	g3dBlendStateMgr::SetBlendState(stp_NoBlend);
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );

	//g3dLightMgrVk::Implementation()->DisableAllLights();
	return m_stats.m_nTriangles;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
int shdwPassAOVolumes::RenderNode(const sNodePlusState& i_Node)
{
	int nTriangles = 0;
	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);

	// resolve material/effect
	const matMaterial* pMaterial = &l_AOMat;//g3dVkUtil::GetMaterial(i_pNode);
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	// note that we still want backface culling here regardless of fragment flag.
	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	fxEffect* pDXEffect = ((effShaderBaseVk*)pEffect)->GetFxEffect();
	pDXEffect->GetVariableByName("g_DblSide")->AsScalar()->SetFloat( 1 );

	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	bool bDoubleSided = i_Node.m_pNode->GetFragment()->GetDoubleSided();
	if (bDoubleSided)
	{
		// render a 2nd time but extrude in opposite direction
		pDXEffect->GetVariableByName("g_DblSide")->AsScalar()->SetFloat( -1 );
		nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );
		pDXEffect->GetVariableByName("g_DblSide")->AsScalar()->SetFloat( 1 );

	}


	return nTriangles;
}

//----------------------------------------------------------------------------------------
// This function renders a single node with the special hair shader
//----------------------------------------------------------------------------------------
int shdwPassAOVolumes::RenderHairNode(const sNodePlusState& i_Node)
{
	// set the minimal state necessary to draw normals.
	int nTriangles = 0;
	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);

	// resolve material/effect
	const matMaterial* pMaterial = &l_DrawHairMat;//g3dVkUtil::GetMaterial(i_pNode);
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

	pEffect->SetTechnique("Tangents");

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	return nTriangles;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPassAOVolumes::BlendAO(g2dRenderTarget* i_pRenderTarget, matRenderTargetTexture* i_pAOSrc )
{
	const g3dPrefs::g3dRenderPrefs& p = g3dPrefs::CurrentPrefs();

	effShaderBaseVk* i_pEffect = (effShaderBaseVk*)matShaderMgr::GetSpecialEffect(("AOVolumes.fx"));
	fxEffect* pEffect = i_pEffect->GetFxEffect();

	i_pEffect->SetTechnique("FinalPass");

	vkdImage* pResView = g3dVkTextureUtil::GetImage(i_pAOSrc);
	pEffect->GetVariableByName("g_AOBuffer")->AsShaderResource()->SetResource(pResView);
	pEffect->GetVariableByName("g_AOTint")->AsVector()->SetFloatVector(m_Params.m_Color.Ptr());
	float factor = 1.0f / (float)(1 << g3dPrefs::CurrentPrefs().m_AOResolutionReduce);
	maVector4d uvscale(factor, factor, 0, 0);
	pEffect->GetVariableByName("g_UVScale")->AsVector()->SetFloatVector(uvscale.Ptr());

    float contrast = m_Params.m_Contrast / (1.0f - sin(m_Params.m_AngleBias * maConstants::c_fAngleToRad));
	pEffect->GetVariableByName("g_Contrast")->AsScalar()->SetFloat( contrast );

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// turn on stencil - was set up in earlier pass.
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_LessE_Ref_Equal, 1 );

	g3dBlendStateMgr::SetBlendState(st_AOBlend);

	// set render target and source texture
	i_pRenderTarget->MakeCurrent();
	int w,h;
	i_pRenderTarget->GetDimensions(w,h);

	int nPasses = i_pEffect->Begin();
	i_pEffect->BeginPass(0);
	g3dVkUtil::DrawFullScreenQuad( w,h );
	i_pEffect->EndPass();
	i_pEffect->End();

	// restore stencil state
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState(st_AOBlendRestore);

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// unbind texture from shader so that it can be set as rendertarget later.
	vkdImage* nullPSR[3] = {NULL, NULL, NULL};
	vkdGlobal::Context().SetShaderResources(0,3,nullPSR);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPassAOVolumes::DrawStencilPass(g2dRenderTarget* i_pTarget)
{
	if (!g2dVkGlobal::HasStencil())
	{
		static bool warnOnce = true;
		if (warnOnce)
		{
			DBG_WARNING("SSAO skipping stencil buffer pass");
			warnOnce = false;
		}
		return;
	}

	i_pTarget->MakeCurrent();

	const uint8_t stencilValue = 1;
	vkdGlobal::Context().ClearDepthStencil(
		g2dVkGlobal::GetDepthTarget(),
		false, 1.0f,
		true, stencilValue);

	// enable stenciling
	// draw objects, filling stencil buffer with stencilValue's where the objects that get AO live.
	// so let the stencil test always pass.
	// if depth + stencil test passes, put the stencil ref value in the buffer
	// set the stencil ref value to 1 or 0 depending if the obj receives occlusion.
	g3dDepthStencilStateMgr::DepthStencilState ds_Saved;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Saved );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_Replace );

	// don't write color! no need!
	g3dBlendStateMgr::SetBlendState(st_NoBlendDepthOnly);

	int i,n;

	const nodeCacheList& nonShadowNodes = m_SceneInfo->GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		m_stats.m_nTriangles += RenderNodeStencil(currentNode);
	}
	const nodeCacheList& shadowNodes = m_SceneInfo->GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		m_stats.m_nTriangles += RenderNodeStencil(currentNode);
	}
	const nodeCacheList& nonSolidNodes = m_SceneInfo->GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		m_stats.m_nTriangles += RenderNodeStencil(currentNode);
	}
	// restore some state.
	g3dDrawStyleUtilVk::RestoreNormalDrawStyle();

	// restore color write mask
	g3dBlendStateMgr::SetBlendState(stp_NoBlend);

	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Saved );
}

//--------------------------------------------------------------------
// clear the mask.
//--------------------------------------------------------------------
void shdwPassAOVolumes::ClearStencil(g2dRenderTarget* i_pTarget)
{
	// disable stencil test and restore some state. should I clear it here? probably.
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );

	// can i get away with not clearing it?
//	HRESULT op_result = g2dDX11Global::g_pDevice->Clear(	0,
//		NULL,
//		D3DCLEAR_STENCIL,
//		0,
//		1.0f,
//		stencilValue);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int shdwPassAOVolumes::RenderNodeStencil(const sNodePlusState& i_Node)
{
	DBG_ASSERT(i_Node.m_pNode->GetFragment() != NULL, "null fragment in SSAO");

	// Set the comparison reference value
	// this is where we decide if the fragment will receive occlusion.
	// either way, we still have to draw the frag so it fills depth buffer for the depthstencil test later.
	bool UseRef = i_Node.m_pNode->GetFragment()->GetReceivesOcclusion();
	g3dDepthStencilStateMgr::SetDepthStencilState( UseRef ? ds_Test_Write_LessE_Ref_Replace : ds_Test_Write_LessE_Replace, UseRef ? 1 : 0 );

	// set the minimal state necessary to draw depth.
	int nTriangles = 0;
	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);

	// resolve material/effect
	const matMaterial* pMaterial = &l_DepthMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

//	pEffect->SetTechnique("ViewSpaceDepthRemap");
	pEffect->SetTechnique("ViewSpaceDepth");

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	bool bDoubleSided = i_Node.m_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	return nTriangles;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPassAOVolumes::BlurAO(g2dRenderTarget* i_pRenderTarget)
{
	const g3dPrefs::g3dRenderPrefs& p = g3dPrefs::CurrentPrefs();

	effShaderBaseVk* i_pEffect = (effShaderBaseVk*)matShaderMgr::GetSpecialEffect(("AOVolumes.fx"));
	fxEffect* pEffect = i_pEffect->GetFxEffect();

	fxEffectTechnique* pTechnique = pEffect->GetTechniqueByName("FinalPassUpsample");

    // Update shader variables g_Resolution and g_InvResolution
	int width, height;
	i_pRenderTarget->GetDimensions(width, height);
	int DBWidth    = width;//m_DepthBuffer->GetWidth();
	int DBHeight   = height;//m_DepthBuffer->GetHeight();
	float invResolution[2];
    invResolution[0] = 1.0f / (float)width;
    invResolution[1] = 1.0f / (float)height;
	float resolution[2];
    resolution[0]    = (float)width;
    resolution[1]    = (float)height;
	float OverscanRatio[2] = {1,1};
	//OverscanRatio[0] = width / (float)DBWidth;
	//OverscanRatio[1] = height / (float)DBHeight;

	pEffect->GetVariableByName("g_InvResolution")->AsVector()->SetFloatVector( invResolution );
	pEffect->GetVariableByName("g_Resolution")->AsVector()->SetFloatVector( resolution );
	pEffect->GetVariableByName("g_OverscanRatio")->AsVector()->SetFloatVector( OverscanRatio );

    float m_EdgeThreshold  = 0.1f;

	// the blur params.
	float radius     = m_Params.m_BlurWidth;
    float sigma      = (radius+1) / 2;
    float inv_sigma2 = 1.0f / (2*sigma*sigma);

	// Blur Pass X : render from ao target into temp target

	if (m_DepthBuffer && g3dPrefs::CurrentPrefs().m_bHDRAA)
	{
		//m_pNearDepthBuffer->MakeDepthCurrent();
		m_DepthBuffer->MakeCurrent();
	}

	matRenderTargetTexture* pTempTarget = m_NormalBuffer;

	pTempTarget->MakeCurrent();
	int w=1,h=1;
	pTempTarget->GetDimensions(w,h);

	pEffect->GetVariableByName("g_BlurFalloff")->AsScalar()->SetFloat( inv_sigma2 );
	pEffect->GetVariableByName("g_BlurRadius")->AsScalar()->SetFloat( radius );

	pEffect->GetVariableByName("g_EdgeThreshold")->AsScalar()->SetFloat( m_EdgeThreshold );
    float sharpness = (m_Params.m_BlurSharpness) * (m_Params.m_BlurSharpness);
	pEffect->GetVariableByName("g_Sharpness")->AsScalar()->SetFloat( sharpness );


	pEffect->GetVariableByName("g_AOTint")->AsVector()->SetFloatVector(m_Params.m_Color.Ptr());

	// the reduced AO framebuffer is the input buffer.
	// scale UVs to read from it.
	float factor = 1.0f / (float)(1 << g3dPrefs::CurrentPrefs().m_AOResolutionReduce);
	maVector4d uvscale(factor, factor, 0, 0);
	pEffect->GetVariableByName("g_UVScale")->AsVector()->SetFloatVector(uvscale.Ptr());

    float contrast = m_Params.m_Contrast / (1.0f - sin(m_Params.m_AngleBias * maConstants::c_fAngleToRad));
	pEffect->GetVariableByName("g_Contrast")->AsScalar()->SetFloat( contrast );

	//pEffect->GetVariableByName("tDepth")->AsShaderResource()->SetResource( g3dVkTextureUtil::GetImage(m_PositionBuffer) );
	//pEffect->GetVariableByName("tColor")->AsShaderResource()->SetResource( NULL );

	g3dBlendStateMgr::SetBlendState(stp_NoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	vkdImage* aoView = g3dVkTextureUtil::GetImage(m_AOBuffer);
	pEffect->GetVariableByName("g_AOBuffer")->AsShaderResource()->SetResource( aoView );
	vkdImage* posView = g3dVkTextureUtil::GetImage(m_PositionBuffer);
	pEffect->GetVariableByName("g_Positions")->AsShaderResource()->SetResource(posView);

	fxEffectPass* pPass0 = pTechnique->GetPassByIndex(0);
	pPass0->Apply(0);
	g3dVkUtil::DrawFullScreenQuad( w,h );

	//matTextureMgr::SaveTextureToFile(pTempTarget, fsLocator(itString("AOVOLUMEBLUR.dds")));

	g3dBlendStateMgr::SetBlendState(st_AOBlend);

	// turn on stencil - was set up in earlier pass.
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_LessE_Ref_Equal, 1 );

	// unbind texture from shader so that it can be set as rendertarget later.
	vkdImage* nullPSR[3] = {NULL, NULL, NULL};
	vkdGlobal::Context().SetShaderResources(0,3,nullPSR);

	// Blur Pass Y : render from temp target back into main AO target
//	m_AOBuffer->MakeCurrent();
	i_pRenderTarget->MakeCurrent();

	aoView = g3dVkTextureUtil::GetImage(pTempTarget);
	pEffect->GetVariableByName("g_AOBuffer")->AsShaderResource()->SetResource( aoView );
	posView = g3dVkTextureUtil::GetImage(m_PositionBuffer);
	pEffect->GetVariableByName("g_Positions")->AsShaderResource()->SetResource(posView);

	fxEffectPass* pPass1 = pTechnique->GetPassByIndex(1);
	pPass1->Apply(0);
	g3dVkUtil::DrawFullScreenQuad( w,h );

	// restore stencil state
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState(st_AOBlendRestore);

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// unbind texture from shader so that it can be set as rendertarget later.
	vkdGlobal::Context().SetShaderResources(0,3,nullPSR);
}

}
