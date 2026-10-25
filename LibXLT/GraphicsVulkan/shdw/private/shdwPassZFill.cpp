#ifndef GVK_SHDW_PASSZFILL_HPP
#include "GraphicsVulkan/shdw/shdwPassZFill.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
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
#ifndef GVK_G3D_FOGVK_HPP
#include "GraphicsVulkan/G3d/g3dFogVk.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif
#ifndef GVK_G3D_TRANSPARENCYSORTVK_HPP
#include "GraphicsVulkan/G3d/g3dTransparencySortVk.hpp"
#endif
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{

static bool s_bUseSolidHairZFill = false;

namespace
{
	matMaterial l_ZFillMat;
	matMaterial l_HairMat;

	g3dBlendStateMgr::BlendState* stp_AddBlend = NULL;
	g3dBlendStateMgr::BlendState* stp_AddNoBlendAlphaOnly = NULL;
	g3dBlendStateMgr::BlendState* stp_AddNoBlendDepthOnly = NULL;
}; // namespace

shdwPassZFill::shdwPassZFill(bool i_writeAlpha, bool i_bDrawTransparent )
	: m_sceneInfo(NULL), m_isWriteAlpha(i_writeAlpha), m_bDrawTransparent( i_bDrawTransparent )
{
	// lazy init so that this material can be reused across instantiations.
	if (!l_ZFillMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/DepthMap.fx"), matShaderMgr::GetSpecialEffect("DepthMap.fx"));
		l_ZFillMat.SetShaderParams(p);
	}
	if (!l_HairMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/HairDefault.fx"), matShaderMgr::GetSpecialEffect("HairDefault.fx"));
		l_HairMat.SetShaderParams(p);
	}
}

shdwPassZFill::~shdwPassZFill()
{
}

int shdwPassZFill::Render(float i_time)
{
	G3D_PROFILE_SCOPE("PassZFill");

	m_stats.Reset();

	int i,n;

	// turn off all color writes! this is depth write only!
	// write alpha=1 for opaque objects too.
	// see depthmap.fx zfill pixel shader.
	if (m_isWriteAlpha)
	{
		g3dBlendStateMgr::SetBlendState(stp_AddNoBlendAlphaOnly);
	}
	else
	{
		// Disable all channel's writing and just write to the z buffer
		g3dBlendStateMgr::SetBlendState(stp_AddNoBlendDepthOnly);
	} 

	const nodeCacheList& nonShadowNodes = m_sceneInfo->GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode, m_sceneInfo->GetRenderStateCache());
	}
	const nodeCacheList& shadowNodes = m_sceneInfo->GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode, m_sceneInfo->GetRenderStateCache());
	}

	const nodeCacheList& nonSolidNodes = m_sceneInfo->GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode, m_sceneInfo->GetRenderStateCache());
	}

	if( m_bDrawTransparent && g3dPrefs::CurrentPrefs().m_bEnableTransparent )
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
				m_stats.m_nTriangles += RenderNode( NS, m_sceneInfo->GetRenderStateCache());
			}
		}
	}

	//hair nodes
#ifdef HAIR_SUPPORTED
	if( s_bUseSolidHairZFill && g3dPrefs::CurrentPrefs().m_bEnableHair )
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

	g3dBlendStateMgr::SetBlendState(stp_AddBlend);
	return m_stats.m_nTriangles;
}

int shdwPassZFill::RenderOneNode(float i_time, const sNodePlusState& i_Node)
{

	m_stats.Reset();

	// turn off all color writes! this is depth write only!
	// write alpha=1 for opaque objects too.
	// see depthmap.fx zfill pixel shader.
	if (m_isWriteAlpha)
	{
		g3dBlendStateMgr::SetBlendState(stp_AddNoBlendAlphaOnly);
	}
	else
	{
		// Disable all channel's writing and just write to the z buffer
		g3dBlendStateMgr::SetBlendState(stp_AddNoBlendDepthOnly);
	} 

	RenderNode(i_Node, m_sceneInfo->GetRenderStateCache());
	
	// restore some state.
	g3dDrawStyleUtilVk::RestoreNormalDrawStyle();

	g3dBlendStateMgr::SetBlendState(stp_AddBlend);
	return m_stats.m_nTriangles;
}

int shdwPassZFill::RenderNode(const sNodePlusState& i_Node, g3dRenderStateTraverser* i_pRenderStateCache, float i_OverWriteAlpha)
{
	int nTriangles = 0;

	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);
	i_pRenderStateCache->SetRenderState( i_Node.m_StateCache, 
		g3dSingleLightRendering::GetDoSingleLightRendering() );

	g3dFogVk::EnableFog( false );

	// resolve material/effect
	const matMaterial* pMaterial = &l_ZFillMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	
	effShaderBaseVk* pEffBase = (effShaderBaseVk*)g3dVkUtil::GetEffect("DepthMap.fx");
	fxEffect* pEffectDX = pEffBase->GetFxEffect();
	pEffectDX->GetVariableByName("alphaValue")->AsScalar()->SetFloat(i_OverWriteAlpha);

	// e_Environment is a different type of ambient pass that is ambient-only.
	pEffect->SetTechnique("ZFill");

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	//setup transparency parameters
	const matMaterial* pOrigMaterial = i_Node.m_pNode->GetFragment()->GetMaterial();
	g3dVkUtil::SetupOpacity(pEffBase, *pOrigMaterial, "hasTransparencyMap", "transparencyMap", "g_transparency");

	bool bDoubleSided = i_Node.m_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	return nTriangles;
}

//----------------------------------------------------------------------------------------
// This function renders a single node with the special hair shader
//----------------------------------------------------------------------------------------
int shdwPassZFill::RenderHairNode(const sNodePlusState& i_Node)
{
	// set the minimal state necessary to draw depth.

	int nTriangles = 0;

	// resolve material/effect
	const matMaterial* pMaterial = &l_HairMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseVk* i_pEffect = (effShaderBaseVk*)pEffect;
	fxEffect* pD3DEffect = i_pEffect->GetFxEffect();

	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	pEffect->SetupMaterial( pMaterial );

#ifdef USE_NORMALIZED_DEPTHS
	pEffect->SetTechnique("ViewSpaceDepthN");
#else
	pEffect->SetTechnique("ViewSpaceDepth");
#endif

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	return nTriangles;
}

void shdwPassZFill::SetWriteAlpha(const bool i_bWriteAlpha)
{
	m_isWriteAlpha = i_bWriteAlpha;
}


void shdwPassZFill::SetDrawTransparent( const bool i_bDrawTransparent )
{
	m_bDrawTransparent = i_bDrawTransparent;
}
void shdwPassZFill::InitStates()
{
	stp_AddBlend = new g3dBlendStateMgr::BlendState(false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, 
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	stp_AddNoBlendAlphaOnly = new g3dBlendStateMgr::BlendState(false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, 
		VK_COLOR_COMPONENT_A_BIT);
	stp_AddNoBlendDepthOnly = new g3dBlendStateMgr::BlendState(false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, 
		0);
}

void shdwPassZFill::CleanupStates()
{
	delete stp_AddBlend; stp_AddBlend = NULL;
	delete stp_AddNoBlendAlphaOnly; stp_AddNoBlendAlphaOnly = NULL;
	delete stp_AddNoBlendDepthOnly; stp_AddNoBlendDepthOnly = NULL;
}

}
