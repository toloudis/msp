/****************************************************************************\
**	shdwPassNormals.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSNORMALS_HPP
#include "GraphicsVulkan/shdw/shdwPassNormals.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
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
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
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
#ifndef GVK_G3D_TRANSPARENCYSORTVK_HPP
#include "GraphicsVulkan/G3d/g3dTransparencySortVk.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

#define TANGENT_NORMALS true

namespace gvk
{

namespace
{
	matMaterial l_DrawNormalMat;
	matMaterial l_DrawHairMat;

	g3dBlendStateMgr::BlendState* stp_MulNoBlendNoAlpha = NULL;
	g3dBlendStateMgr::BlendState* stp_NoBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* dsp_Test_Write_LessE_NS = NULL;
}; // namespace

void shdwPassNormals::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	stp_MulNoBlendNoAlpha = new g3dBlendStateMgr::BlendState(false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT);
	stp_NoBlend = new g3dBlendStateMgr::BlendState(false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);;

	dsp_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState(
		true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT);
}

void shdwPassNormals::CleanupStates()
{
	delete stp_MulNoBlendNoAlpha;	stp_MulNoBlendNoAlpha = NULL;
	delete stp_NoBlend;				stp_NoBlend = NULL;
	delete dsp_Test_Write_LessE_NS;	dsp_Test_Write_LessE_NS = NULL;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassNormals::shdwPassNormals()
	: m_sceneInfo(NULL), m_pCamera(NULL), m_RenderPositions(false)
{
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassNormals::shdwPassNormals(g2dRenderTarget* i_pDepthTarget, const camCamera* i_pCamera)
	: m_sceneInfo(NULL), m_pCamera(i_pCamera), m_RenderPositions(false)
{
	m_pRenderTarget = i_pDepthTarget;

	InitializeMaterial();
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassNormals::~shdwPassNormals()
{
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassNormals::InitializeMaterial()
{
	// lazy init so that this material can be reused across instantiations.
	if (!l_DrawNormalMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/NormalMap.fx"), matShaderMgr::GetSpecialEffect("NormalMap.fx"));
		l_DrawNormalMat.SetShaderParams(p);
	}

	if (!l_DrawHairMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/HairDefault.fx"), matShaderMgr::GetSpecialEffect("HairDefault.fx"));
		l_DrawHairMat.SetShaderParams(p);
	}
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
int shdwPassNormals::Render(float i_time)
{
	G3D_PROFILE_SCOPE("PassNormals");
	m_stats.Reset();

	m_pRenderTarget->MakeCurrent();

	if (m_RenderPositions)
		m_pRenderTarget->Clear(maFloatRGBA(10000,10000,10000,0));
	else
		m_pRenderTarget->Clear(maFloatRGBA(0,0,0,0));

	m_pRenderTarget->ClearDepthStencil(1.0f, g2dVkGlobal::HasStencil(), 0);

	g3dBlendStateMgr::SetBlendState(stp_MulNoBlendNoAlpha);

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	int i,n;

	const nodeCacheList& nonShadowNodes = m_sceneInfo->GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode, TANGENT_NORMALS, m_RenderPositions);
	}
	const nodeCacheList& shadowNodes = m_sceneInfo->GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode, TANGENT_NORMALS, m_RenderPositions);
	}
	const nodeCacheList& nonSolidNodes = m_sceneInfo->GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode, TANGENT_NORMALS, m_RenderPositions);
	}
	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
	{
		g3dTransparencySortVk* pTransNodes = m_sceneInfo->GetTransparentNodes();
		const TranspNodeVector& tNodes = pTransNodes->GetTransparentNodes();
		TranspNodeVector::const_iterator it, end = tNodes.end();

		for (it = tNodes.begin(); it != end; ++it)
		{
			sNodePlusState NS;
			NS.m_pNode = it->m_pSceneNode;
			NS.m_StateCache = it->m_RenderStateCache;

			bool doRender = true;

			g3dFragment * frag = (g3dFragment*)NS.m_pNode->GetFragment();
			if ( frag )
			{
				matMaterial * mat = frag->GetMaterial();
				if ( mat )
				{
					doRender = !mat->GetBelongsToLightShaft();
				}
			}

			if ( doRender )
			{
				m_stats.m_nTriangles += RenderNode( NS, TANGENT_NORMALS, m_RenderPositions );
			}
		}
	}

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
	g3dDrawStyleUtilVk::RestoreNormalDrawStyle();

	// restore blend stae, can be drop after reorg
	g3dBlendStateMgr::SetBlendState(stp_NoBlend);

	//g3dLightMgrVk::Implementation()->DisableAllLights();
	return m_stats.m_nTriangles;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
int shdwPassNormals::RenderNode(const sNodePlusState& i_Node, bool i_isTanSpace, bool i_bRenderPositions)
{
	int nTriangles = 0;
	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);

	// resolve material/effect
	const matMaterial* pMaterial = &l_DrawNormalMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseVk* i_pEffect = (effShaderBaseVk*)pEffect;
	fxEffect* pD3DEffect = i_pEffect->GetFxEffect();

	if (i_bRenderPositions)
	{
		pEffect->SetTechnique("ViewSpacePos");
	}
	else if (i_isTanSpace)
	{
		pEffect->SetTechnique("ViewSpaceNormal");
	}
	else
	{
		pEffect->SetTechnique("WorldSpaceNormal");
	}

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	//setup transparency parameters
	const matMaterial* pOrigMaterial = i_Node.m_pNode->GetFragment()->GetMaterial();
	g3dVkUtil::SetupOpacity(i_pEffect, *pOrigMaterial, "hasTransparencyMap", "TransparencyMap", "g_Transparency");

	bool bDoubleSided = i_Node.m_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	return nTriangles;
}

//----------------------------------------------------------------------------------------
// This function renders a single node with the special hair shader
//----------------------------------------------------------------------------------------
int shdwPassNormals::RenderHairNode(const sNodePlusState& i_Node)
{
	// set the minimal state necessary to draw normals.
	int nTriangles = 0;
	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);

	// resolve material/effect
	const matMaterial* pMaterial = &l_DrawHairMat;//g3dVkUtil::GetMaterial(i_pNode);
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

	pEffect->SetTechnique("Tangents");
//	pEffect->SetTechnique("Simple");

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	pMaterial->StrandHairData().m_SubPixelPower = g3dPrefs::CurrentPrefs().m_HairSubPixelPower;

	pEffect->SetupMaterial( pMaterial );

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	return nTriangles;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassNormals::SetRenderPositions(bool i_bWritePositions)
{
	m_RenderPositions = i_bWritePositions;
}

}
