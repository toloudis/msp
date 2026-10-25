/****************************************************************************\
**	shdwPassDepth.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSDEPTH_HPP
#include "GraphicsVulkan/shdw/shdwPassDepth.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
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
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef MAT_TEXTURE_HPP
#include "Graphics/mat/matTexture.hpp"
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
#ifndef GVK_G3DLIGHTMGRVK_HPP
#include "GraphicsVulkan/G3d/g3dLightMgrVk.hpp"
#endif
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_G3D_TRANSPARENCYSORTVK_HPP
#include "GraphicsVulkan/G3d/g3dTransparencySortVk.hpp"
#endif
#ifndef GVK_FXEFFECTVK_HPP
#include "GraphicsVulkan/Fx/fxEffectVk.hpp"
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

namespace gvk
{

namespace
{
	matMaterial l_DepthMat;
	matMaterial l_HairMat;

	g3dBlendStateMgr::BlendState* stp_RenderInit = NULL;
	g3dBlendStateMgr::BlendState* stp_NoColor = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* dsp_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* dsp_Test_Write_LessE_NS = NULL;
}; // namespace

void shdwPassDepth::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	stp_RenderInit = new g3dBlendStateMgr::BlendState(false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	stp_NoColor = new g3dBlendStateMgr::BlendState(false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, 0 );
	dsp_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState(
		false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT);
	dsp_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState(
		true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT);
}

void shdwPassDepth::CleanupStates()
{
	delete stp_RenderInit;
	delete stp_NoColor;
	delete dsp_Disable_NS;
	delete dsp_Test_Write_LessE_NS;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassDepth::shdwPassDepth()
: m_sceneInfo(NULL), m_pOrigCamera( NULL ), 
m_pSrcDepthBuffer( NULL ), m_bCompareLess( false ), m_bRenderInvisible( false ),
m_bOnlyDepths( false )
{
	m_eTechnique = eDefault;
}

shdwPassDepth::shdwPassDepth( g2dRenderTarget* i_pDepthTarget, const camCamera* i_pCamera, bool i_bRenderInvisible, matTexture* i_pSrcDepth, eTechnique i_eTech, bool i_bOnlyDepths )
	: m_sceneInfo(NULL), m_pOrigCamera( i_pCamera ), 
	m_pSrcDepthBuffer( i_pSrcDepth ), m_eTechnique( i_eTech ), m_bRenderInvisible(i_bRenderInvisible),
	m_Camera( *i_pCamera ), m_bOnlyDepths( i_bOnlyDepths )
{
	m_pRenderTarget = i_pDepthTarget;
	m_Near = i_pCamera->GetNearClip();
	m_Far = i_pCamera->GetFarClip();

	//copy the camera so we can modify it for overscan;
//	m_Camera = camCamera( *i_pCamera );

	// lazy init so that this material can be reused across instantiations.
	if (!l_DepthMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/DepthRender.fx"), matShaderMgr::GetSpecialEffect("DepthRender.fx"));
		l_DepthMat.SetShaderParams(p);
	}

	if (!l_HairMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/HairDefault.fx"), matShaderMgr::GetSpecialEffect("HairDefault.fx"));
		l_HairMat.SetShaderParams(p);
	}

	m_OverscanPixels = 0;
}
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassDepth::~shdwPassDepth()
{
}

//----------------------------------------------------------------------------------------
// This function renders non-transparent objects depth values into the target
// Note that the target will always be cleared.
//----------------------------------------------------------------------------------------
int shdwPassDepth::Render(float i_time)
{
	G3D_PROFILE_SCOPE("PassDepth");
	m_stats.Reset();

	m_pRenderTarget->MakeCurrent();

	g3dBlendStateMgr::BlendState st_OldBlend;
	g3dBlendStateMgr::GetCurrentBlendState( st_OldBlend );

	if( m_bOnlyDepths )  g3dBlendStateMgr::SetBlendState( stp_NoColor );
	else                 g3dBlendStateMgr::SetBlendState( stp_RenderInit );

	SetupMinimalNearFar();

	//set altered projection camera
	g3dSceneRenderUtil::SetViewingTransforms(m_Camera);

	int i,n;
	const nodeCacheList& nonShadowNodes = m_sceneInfo->GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}
	const nodeCacheList& shadowNodes = m_sceneInfo->GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}
	const nodeCacheList& nonSolidNodes = m_sceneInfo->GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode);
	}
	
	/*if (m_bRenderInvisible)
	{
		const nodeCacheList& invisibleMaskBlackNodes = m_sceneInfo->GetInvisibleMaskBlackNodes();
		n = invisibleMaskBlackNodes.size();
		for (i = 0; i < n; i++)
		{
			const sNodePlusState& currentNode = invisibleMaskBlackNodes[i];
			m_stats.m_nTriangles += RenderNode(currentNode);		
		}
	}*/

	// restore some state.
	g3dBlendStateMgr::SetBlendState( &st_OldBlend );
	g3dDrawStyleUtilVk::RestoreNormalDrawStyle();

	//restore original camera transform
	g3dSceneRenderUtil::SetViewingTransforms(*m_pOrigCamera);

	return m_stats.m_nTriangles;
}

//----------------------------------------------------------------------------------------
// This function renders only transparent objects depth values into the target
// Note that the target will only be cleared only in reversed mode (near to far)
//----------------------------------------------------------------------------------------
int shdwPassDepth::RenderTransparent()
{
	m_stats.Reset();

	m_pRenderTarget->MakeCurrent();

	g3dBlendStateMgr::BlendState st_OldBlend;
	g3dBlendStateMgr::GetCurrentBlendState( st_OldBlend );

	if( m_bOnlyDepths )  g3dBlendStateMgr::SetBlendState( stp_NoColor );
	else                 g3dBlendStateMgr::SetBlendState( stp_RenderInit );

	SetupMinimalNearFar();

	//set altered projection camera
	g3dSceneRenderUtil::SetViewingTransforms(m_Camera);

	g3dTransparencySortVk* pTranspNodes = m_sceneInfo->GetTransparentNodes();
	const TranspNodeVector& tNodes = pTranspNodes->GetTransparentNodes();
	TranspNodeVector::const_iterator it, end = tNodes.end();

	for (it = tNodes.begin(); it != end; ++it)
	{
		sNodePlusState NS;
		NS.m_pNode = it->m_pSceneNode;
		NS.m_StateCache = it->m_RenderStateCache;
		m_stats.m_nTriangles += RenderNode( NS );
	}

	/*if (m_bRenderInvisible)
	{
		const nodeCacheList& invisibleMaskBlackTransparentNodes = m_sceneInfo->GetInvisibleMaskTransparentNodes();
		int n = invisibleMaskBlackTransparentNodes.size();
		for (int i = 0; i < n; i++)
		{
			const sNodePlusState& currentNode = invisibleMaskBlackTransparentNodes[i];
			m_stats.m_nTriangles += RenderNode(currentNode);
		}
	}*/

	// restore some state.
	g3dBlendStateMgr::SetBlendState( &st_OldBlend );
	g3dDrawStyleUtilVk::RestoreNormalDrawStyle();

	//restore original camera transform
	g3dSceneRenderUtil::SetViewingTransforms(*m_pOrigCamera);

	return m_stats.m_nTriangles;
}

//----------------------------------------------------------------------------------------
// This function renders only hair objects depth values into the target
// Note that the target will only be cleared in reversed mode (near to far)
//----------------------------------------------------------------------------------------
int shdwPassDepth::RenderHair()
{
	m_stats.Reset();

	m_pRenderTarget->MakeCurrent();

	g3dBlendStateMgr::BlendState st_OldBlend;
	g3dBlendStateMgr::GetCurrentBlendState( st_OldBlend );

	if( m_bOnlyDepths )  g3dBlendStateMgr::SetBlendState( stp_NoColor );
	else                 g3dBlendStateMgr::SetBlendState( stp_RenderInit );

	SetupMinimalNearFar();

	//set altered projection camera
	g3dSceneRenderUtil::SetViewingTransforms(m_Camera);

	//hair nodes
#ifdef HAIR_SUPPORTED
	if (g3dPrefs::CurrentPrefs().m_bEnableHair)
	{
		g3dTransparencySortVk* pHairNodes = m_sceneInfo->GetHairNodes();
		const TranspNodeVector& tHNodes = pHairNodes->GetTransparentNodes();
		TranspNodeVector::const_iterator it, end = tHNodes.end();

		for (it = tHNodes.begin(); it != end; ++it)
		{
			sNodePlusState NS;
			NS.m_pNode = it->m_pSceneNode;
			NS.m_StateCache = it->m_RenderStateCache;
			m_stats.m_nTriangles += RenderHairNode( NS );
		}
	}
#endif//HAIR_SUPPORTED

	// restore some state.
	g3dBlendStateMgr::SetBlendState( &st_OldBlend );
	g3dDrawStyleUtilVk::RestoreNormalDrawStyle();

	//restore original camera transform
	g3dSceneRenderUtil::SetViewingTransforms(*m_pOrigCamera);

	return m_stats.m_nTriangles;
}


//----------------------------------------------------------------------------------------
// This function renders a single node
// if no source is specified then all depths will be rendered
//   (an optional transparency value will be tested to remove non solid pixels)
// otherwise one of two depth peeling modes will be used
//  if reversed then only depths that are farther away from the source will be stored (for back to front sorting or farthest z)
//  else only depths that are closer than the source will be stored (for front to back sorting or nearest z)
//----------------------------------------------------------------------------------------
int shdwPassDepth::RenderNode(const sNodePlusState& i_Node)
{
	// set the minimal state necessary to draw depth.

	int nTriangles = 0;

	// resolve material/effect
	const matMaterial* pMaterial = &l_DepthMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseVk* i_pEffect = (effShaderBaseVk*)pEffect;
	fxEffect* pD3DEffect = i_pEffect->GetFxEffect();

	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	const matMaterial* pOrigMaterial = i_Node.m_pNode->GetFragment()->GetMaterial();
	g3dVkUtil::SetupOpacity(i_pEffect, *pOrigMaterial, "hasTransparencyMap", "TransparencyMap", "g_Transparency");

	if( m_pSrcDepthBuffer )
	{
		pD3DEffect->GetVariableByName("DepthMap")->AsShaderResource()->SetResource(
						g3dVkTextureUtil::GetImage(m_pSrcDepthBuffer));
	}

	int dWidth, dHeight;
	m_pRenderTarget->GetDimensions( dWidth, dHeight );

	float InvResololution[2] = {0};
	InvResololution[0] = 1.0f / dWidth;
	InvResololution[1] = 1.0f / dHeight;
	pD3DEffect->GetVariableByName("g_InvScreenSize")->AsVector()->SetFloatVector(InvResololution);

	float bias = 0.0f;
//	float AlphaLevel = 254.0f/255.0f;

	switch( m_eTechnique )
	{
		case eDefault:
		{
			pEffect->SetTechnique("Default");
			break;
		}
		case eScreenPeelLess:
		{
			pEffect->SetTechnique("ScreenSpaceDepthPeelLess");
			bias = 0.00001f;
//			AlphaLevel = 1.0f/255.0f;
			break;
		}
		case eScreenPeelGreater:
		{
			pEffect->SetTechnique("ScreenSpaceDepthPeelGreater");
			bias = 0.00001f;
//			AlphaLevel = 1.0f/255.0f;
			break;
		}
		case eScreenBiased:
		{
			bias = 0.00001f;
			pEffect->SetTechnique("Default");
			break;
		}
		case eScreenAlpha:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("Default");
			break;
		}
		case eView:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("ViewSpaceDepth");
			break;
		}
		case eViewPeelLess:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("ViewSpaceDepthPeelLess");
			break;
		}
		case eViewPeelGreater:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("ViewSpaceDepthPeelGreater");
			break;
		}
		case eNView:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("NViewSpaceDepth");
			break;
		}
		case eNViewPeelLess:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("NViewSpaceDepthPeelLess");
			break;
		}
		case eNViewPeelGreater:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("NViewSpaceDepthPeelGreater");
			break;
		}
	}
	pD3DEffect->GetVariableByName("g_bias")->AsScalar()->SetFloat(bias);
//	pD3DEffect->GetVariableByName("g_alphaThreshold")->AsScalar()->SetFloat(AlphaLevel);


	bool bDoubleSided = i_Node.m_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	//unbind texture to prevent D3D read/write dependency error
	//pD3DEffect->GetVariableByName("DepthMap")->AsShaderResource()->SetResource(NULL);
	vkdImage* nullTex[2] = {NULL,NULL};
	vkdGlobal::Context().SetShaderResources(0, 2, nullTex);

	return nTriangles;
}

//----------------------------------------------------------------------------------------
// This function renders a single node with the special hair shader
//----------------------------------------------------------------------------------------
int shdwPassDepth::RenderHairNode(const sNodePlusState& i_Node)
{
	// set the minimal state necessary to draw depth.

	int nTriangles = 0;

	// resolve material/effect
	const matMaterial* pMaterial = &l_HairMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseVk* i_pEffect = (effShaderBaseVk*)pEffect;
	fxEffect* pD3DEffect = i_pEffect->GetFxEffect();

	effStrandHairData& HairData = pMaterial->StrandHairData();
	HairData.Default();

	int W, H;
	m_pRenderTarget->GetDimensions( W, H );
	HairData.m_InvScreenSize = maVector2d( 1.0f / W, 1.0f / H );

	if( 0==g3dPrefs::CurrentPrefs().m_HairTransparencyMode )	//if solid set power high to disable alpha
	{
		HairData.m_SubPixelPower = 1000000.0f;
	}
	else
	{
		HairData.m_SubPixelPower = g3dPrefs::CurrentPrefs().m_HairSubPixelPower;
	}

	HairData.m_ZNear = m_Near;
	HairData.m_ZFar = m_Far;

	//set depth map
	HairData.m_pDepthTexture = m_pSrcDepthBuffer;

	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	// shading data
	pEffect->SetupMaterial(pMaterial);

	const matMaterial* pOrigMaterial = i_Node.m_pNode->GetFragment()->GetMaterial();
	g3dVkUtil::SetupOpacity(i_pEffect, *pOrigMaterial, "hasTransparencyMap", "TransparencyMap", "g_Transparency");

	if( m_pSrcDepthBuffer )
	{
		pD3DEffect->GetVariableByName("DepthMap")->AsShaderResource()->SetResource(
		g3dVkTextureUtil::GetImage(m_pSrcDepthBuffer));
	}

	int dWidth, dHeight;
	m_pRenderTarget->GetDimensions( dWidth, dHeight );

	float InvResololution[2];
	InvResololution[0] = 1.0f / dWidth;
	InvResololution[1] = 1.0f / dHeight;
	pD3DEffect->GetVariableByName("g_InvScreenSize")->AsVector()->SetFloatVector(InvResololution);

	float bias = 0.0f;
//	float AlphaLevel = 254.0f/255.0f;

	switch( m_eTechnique )
	{
		case eDefault:
		{
			pEffect->SetTechnique("Default");
			break;
		}
		case eScreenPeelLess:
		{
			pEffect->SetTechnique("ScreenSpaceDepthPeelLess");
			bias = 0.00001f;
//			AlphaLevel = 1.0f/255.0f;
			break;
		}
		case eScreenPeelGreater:
		{
			pEffect->SetTechnique("ScreenSpaceDepthPeelGreater");
			bias = 0.00001f;
//			AlphaLevel = 1.0f/255.0f;
			break;
		}
		case eScreenBiased:
		{
			bias = 0.00001f;
			pEffect->SetTechnique("Default");
			break;
		}
		case eScreenAlpha:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("Default");
			break;
		}
		case eView:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("ViewSpaceDepth");
			break;
		}
		case eViewPeelLess:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("ViewSpaceDepthPeelLess");
			break;
		}
		case eViewPeelGreater:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("ViewSpaceDepthPeelGreater");
			break;
		}
		case eNView:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("NViewSpaceDepth");
			break;
		}
		case eNViewPeelLess:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("NViewSpaceDepthPeelLess");
			break;
		}
		case eNViewPeelGreater:
		{
//			AlphaLevel = 1.0f/255.0f;
			pEffect->SetTechnique("NViewSpaceDepthPeelGreater");
			break;
		}
	}

	pD3DEffect->GetVariableByName("g_bias")->AsScalar()->SetFloat(bias);
//	pD3DEffect->GetVariableByName("g_alphaThreshold")->AsScalar()->SetFloat(AlphaLevel);

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	//unbind texture to prevent D3D read/write dependancy error
	pD3DEffect->GetVariableByName("DepthMap")->AsShaderResource()->SetResource(NULL);

	return nTriangles;
}


//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassDepth::SetupMinimalNearFar()
{
#if 0
	// total bounds of all objects to be rendered
	maAxisBox bounds;
	bounds.Union(m_sceneInfo->m_NonShadowNodesBounds);
	bounds.Union(m_sceneInfo->m_ShadowNodesBounds);
	bounds.Union(m_sceneInfo->m_NonSolidNodesBounds);
	// put bounds into cam space
	maAxisBox camBounds = g3dVkUtil::XForm(bounds, g3dSceneGlobal::GetCameraTransform());
	// get near/far and compare with camera near/far.
	float bn = camBounds.GetMinZ();
	float bf = camBounds.GetMaxZ();
	m_Near = m_pCamera->GetNearClip();
	m_Far = m_pCamera->GetFarClip();
	if (bn > m_Near)
		m_Near = bn;
	if (bf < m_Far)
		m_Far = bf;
	// now m_Near and m_Far are the tightest allowable camera space z bounds. 
	// we are going to map [near,far] to [-1,1] to maximize fp precision
	// z' = ((z-N)/(F-N)) * 2 - 1
	// in the shader! and all shaders that need to extract z:
	// z = ((z'+1)*0.5)*(F-N) + N

	matShaderEffect* pEffect = matShaderMgr::GetEffect(l_DepthMat);
	int i = pEffect->GetParamIndex("ZNear");
	pEffect->SetFloat(i, m_Near);
	i = pEffect->GetParamIndex("ZFar");
	pEffect->SetFloat(i, m_Far);
#endif
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassDepth::AdjustCameraOverscan()
{
	int Width, Height;
	m_pRenderTarget->GetDimensions( Width, Height );
	
	float WidthRatio = Width / (float)(Width-(m_OverscanPixels<<1));
	float HeightRatio = Height / (float)(Height-(m_OverscanPixels<<1));

	// add buffer to subviewport
	// assuming the standard vprt is -1..1, and we now have -widthRatio..widthRatio,
	// and assuming widthRatio and heightratio >= 1 :
	float dx = WidthRatio-1.0f;
	float dy = HeightRatio-1.0f;
	float vt=-1,vb=1,vl=-1,vr=1;
	m_pOrigCamera->GetSubViewport(vt,vb,vl,vr);
	m_Camera.SetSubViewport(vt-dy, vb+dy, vl-dx, vr+dx);
	m_Camera.Set();
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassDepth::ColorTexture(g2dRenderTarget* i_pRenderTarget, maFloatRGBA& i_Color )
{
	const g3dPrefs::g3dRenderPrefs& p = g3dPrefs::CurrentPrefs();

	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");
	int technique = pEffect->FindTechnique("SimpleColor");

	g3dBlendStateMgr::BlendState st_OldBlend;
	g3dBlendStateMgr::GetCurrentBlendState( st_OldBlend );
	g3dBlendStateMgr::SetBlendState( stp_RenderInit );

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Disable_NS );

	// set render target and source texture
	i_pRenderTarget->MakeCurrent();
	int w,h;
	i_pRenderTarget->GetDimensions(w,h);

	pEffect->SetFloatVector(pEffect->FindConstant("g_ColorTint"), i_Color.Ptr());

	pEffect->Apply(technique, 0);
	g3dVkUtil::DrawFullScreenQuad( w,h );
	

	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState( &st_OldBlend );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
}

}
