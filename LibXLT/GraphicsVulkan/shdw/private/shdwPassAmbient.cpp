#ifndef GVK_SHDW_PASSAMBIENT_HPP
#include "GraphicsVulkan/shdw/shdwPassAmbient.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

#include "Graphics/g3d/g3dConditionalCompile.hpp"
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
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
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{

namespace
{
	g3dBlendStateMgr::BlendState* st_BlendNoAlpha = NULL;
}

void shdwPassAmbient::InitStates()
{
	/*st_BlendNoAlpha = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, 
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT);*/
	st_BlendNoAlpha = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ZERO,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ZERO,VK_BLEND_OP_ADD, 
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT);
}

void shdwPassAmbient::CleanupStates()
{
	delete st_BlendNoAlpha;
}

shdwPassAmbient::shdwPassAmbient()
	: m_sceneInfo(NULL)
{
}

shdwPassAmbient::~shdwPassAmbient()
{
}

int shdwPassAmbient::Render(float i_time)
{
	G3D_PROFILE_SCOPE("PassAmbient");
	m_stats.Reset();

	int i,n;

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
	// restore some state.
	g3dDrawStyleUtilVk::RestoreNormalDrawStyle();

	//g3dLightMgrVk::Implementation()->DisableAllLights();
	return m_stats.m_nTriangles;
}

int shdwPassAmbient::RenderNode(const sNodePlusState& i_Node, g3dRenderStateTraverser* i_pRenderStateCache)
{
	int nTriangles = 0;
	
	g3dBlendStateMgr::SetBlendState(st_BlendNoAlpha);

	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);
	i_pRenderStateCache->SetRenderState( i_Node.m_StateCache, 
		g3dSingleLightRendering::GetDoSingleLightRendering() );
	nTriangles += g3dSceneRenderUtil::DrawNodeAmbient(i_Node.m_pNode, 
		i_pRenderStateCache->GetEnvironmentState(i_Node.m_StateCache));
	
	return nTriangles;
}

}
