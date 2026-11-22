/****************************************************************************\
**	shdwPassDOF.cpp
**
**	The depth of field prep pass: writes each pixel's blurriness (from
**	its depth and the camera's DOF distances) into the target's alpha.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSDOF_HPP
#include "GraphicsVulkan/shdw/shdwPassDOF.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
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
#ifndef GVK_G3D_TRANSPARENCYSORTVK_HPP
#include "GraphicsVulkan/G3d/g3dTransparencySortVk.hpp"
#endif
#ifndef GVK_SHDW_PASSTRANSPARENT_HPP
#include "GraphicsVulkan/shdw/shdwPassTransparent.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{

namespace
{
	g3dBlendStateMgr::BlendState* st_Initial = NULL;
	g3dBlendStateMgr::BlendState* st_BlendDOF = NULL;
	g3dBlendStateMgr::BlendState* st_Restore = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
}

shdwPassDOF::shdwPassDOF()
	: m_sceneInfo(NULL)
{
}

shdwPassDOF::~shdwPassDOF()
{
}

int shdwPassDOF::Render(float i_time)
{
	G3D_PROFILE_SCOPE("PassDOF");

	m_stats.Reset();

	// FILL ALPHA ONLY WITH BLURRINESS FACTOR
	g3dSingleLightRendering::SetDoDOFPrepPass(true);

	// set all alphas to 127 (0.5) - no blur
	//DX11 commented out
//	g2dDX11Global::g_pDevice->SetVertexShader(NULL);
//	g2dDX11Global::g_pDevice->SetPixelShader(NULL);
	g3dBlendStateMgr::SetBlendState(st_Initial, maFloatRGBA(0.5f,0.5f,0.5f,0.5f)); // or should it be 0x80808080?

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dVkUtil::DrawFullScreenQuad(0.0f, 0.0f, 1.0f, 1.0f);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dBlendStateMgr::SetBlendState(st_BlendDOF);

	if (m_sceneInfo->GetLayer() == NULL)
		goto DOFPassCleanup;

	{
	// draw geometry

	const camCamera* camera = m_sceneInfo->GetCamera();
	// Set the camera and projection transform
	g3dSceneRenderUtil::SetViewingTransforms(*camera);
	// Z Buffering
//	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	int i,n;

	const nodeCacheList& nonShadowNodes = m_sceneInfo->GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];

		g3dDrawStyleUtilVk::SetDrawStyle(currentNode.m_drawStyle);
		m_stats.m_nTriangles += DrawNodeDOF(currentNode.m_pNode);
	}
	const nodeCacheList& shadowNodes = m_sceneInfo->GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];

		g3dDrawStyleUtilVk::SetDrawStyle(currentNode.m_drawStyle);
		m_stats.m_nTriangles += DrawNodeDOF(currentNode.m_pNode);
	}
	const nodeCacheList& nonSolidNodes = m_sceneInfo->GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];

		g3dDrawStyleUtilVk::SetDrawStyle(currentNode.m_drawStyle);
		m_stats.m_nTriangles += DrawNodeDOF(currentNode.m_pNode);
	}

	// is there a better way to handle transparent stuff for DOF?
	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
	{
		g3dSingleLightRendering::SetDoTransparentPass(true);

//		g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_LessE_NS );

		g3dTransparencySortVk* transparencySort = m_sceneInfo->GetTransparentNodes();
		const TranspNodeVector& transpNodes = transparencySort->GetTransparentNodes();
		n = transpNodes.size();
		for (i = 0; i < n; i++)
		{
			const TranspNode& currentNode = transpNodes[i];

			m_stats.m_nTriangles += DrawNodeDOF(currentNode.m_pSceneNode);
		}

//		g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

		g3dSingleLightRendering::SetDoTransparentPass(false);
	}
	}

DOFPassCleanup:
	// restore some state.
	g3dDrawStyleUtilVk::RestoreNormalDrawStyle();
	g3dSingleLightRendering::SetDoDOFPrepPass(false);

	g3dBlendStateMgr::SetBlendState(st_Restore);
	return m_stats.m_nTriangles;
}

int shdwPassDOF::DrawNodeDOF(const g3dSceneNode* i_pNode)
{
	// resolve material/effect
	const matMaterial* pMaterial = g3dVkUtil::GetMaterial(i_pNode);//m_pAlphaMaskMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

	pEffect->SetTechnique(matShaderEffect::e_DOFPrep);
	pEffect->SetupDOFPrep();

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_pNode);

	pEffect->SetupMaterial( pMaterial );

	bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	int nTriangles = g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
	return nTriangles;
}

void shdwPassDOF::InitStates()
{
	const VkColorComponentFlags writeAll = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	st_Initial = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_CONSTANT_COLOR,VK_BLEND_FACTOR_ZERO,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_CONSTANT_COLOR,VK_BLEND_FACTOR_ZERO,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_A_BIT );
	st_BlendDOF = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_A_BIT);
	st_Restore = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		writeAll );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_NEVER, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwPassDOF::CleanupStates()
{
	delete st_Initial;				st_Initial = NULL;
	delete st_BlendDOF;				st_BlendDOF = NULL;
	delete st_Restore;				st_Restore = NULL;
	delete ds_Test_Write_LessE_NS;	ds_Test_Write_LessE_NS = NULL;
	delete ds_Test_LessE_NS;		ds_Test_LessE_NS = NULL;
	delete ds_Disable_NS;			ds_Disable_NS = NULL;
}

}
