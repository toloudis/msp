#ifndef GVK_SHDW_PASSLIT_HPP
#include "GraphicsVulkan/shdw/shdwPassLit.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif
#ifndef G3D_LAYER_HPP
#include "Graphics/g3d/g3dLayer.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_PROJECTEDLIGHT_HPP
#include "Graphics/g3d/g3dProjectedLight.hpp"
#endif
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
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
#ifndef GVK_G3DLIGHTMGRVK_HPP
#include "GraphicsVulkan/G3d/g3dLightMgrVk.hpp"
#endif
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
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
	g3dBlendStateMgr::BlendState* st_Blend = NULL;
}

void shdwPassLit::InitStates()
{
	st_Blend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, 
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT);
}

void shdwPassLit::CleanupStates()
{
	delete st_Blend;
}

shdwPassLit::shdwPassLit()
:	m_sceneInfo(NULL)
{
}
shdwPassLit::~shdwPassLit()
{
}

int shdwPassLit::Render(float i_time)
{
	G3D_PROFILE_SCOPE("PassLit");
	m_stats.Reset();

	if (!g3dSingleLightRendering::GetDoSingleLightRendering())
		return 0;

	const nodeCacheList& shadowNodes = m_sceneInfo->GetShadowNodes();
    // instead, should check if no shadow casting nodes
	if (shadowNodes.empty()) 
		return 0;

	// lights
	const std::vector<g3dLight*>& lights = g3dLightMgrVk::Implementation()->GetLights( );

	// instead, should check if no shadow casting lights
	int num_lights = lights.size();
	if (num_lights == 0) 
		return 0;


	// Disable all lights and then turn them on one at a time
	g3dLightMgrVk::Implementation()->DisableAllLights();

	// Set mode to additive with alpha blend so that
	// each light's contribution adds to the color
	// already in the buffer
	//
	// alpha blendenable always true, this could be taken out

	int num_nodes = shadowNodes.size();
	this->LightingLoopPerLight(lights, shadowNodes, m_sceneInfo->GetRenderStateCache());
//	this->LightingLoopPerNode(lights, shadowNodes, m_sceneInfo->GetRenderStateCache());

	return m_stats.m_nTriangles;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPassLit::LightingLoopPerLight(const std::vector<g3dLight*>& i_Lights, const nodeCacheList& i_Nodes, g3dRenderStateTraverser* i_RenderStateCache)
{
	int i, j, n;
	// draw opaques per-light, shaded, and add to hdr buffer
	for (i = 0; i < i_Lights.size(); i++)
	{
		g3dLight* pLight = i_Lights[i];
		if ( !pLight->IsEnabled() ) 
			continue;
		if ( !pLight->GetCastsShadow() ) 
			continue;

		const g3dProjectedLight* proj_light = dynamic_cast<const g3dProjectedLight*>(pLight);
		if (proj_light)
		{	
			if (!g3dPrefs::CurrentPrefs().m_bProjLightsOn)
				continue;
		}
		else if (!g3dPrefs::CurrentPrefs().m_bPtLightsOn)
			continue;


		g3dSingleLightRendering::SetActiveLight( pLight );
		g3dLightMgrVk::Implementation()->SetLight( pLight );
		g3dLightMgrVk::Implementation()->EnableLight( pLight );

		n = i_Nodes.size();
		for (j = 0; j < n; j++)
		{
			if (!i_Nodes[j].m_pNode->GetFragment()->IsShadowHull())
			{
				if (i_RenderStateCache->IsLightInState(i_Nodes[j].m_StateCache, pLight))
				{
					if (g3dPrefs::CurrentPrefs().m_bProjLightFrustumCull)
					{
						if (proj_light != NULL)
						{
							if (g3dSceneRenderUtil::get_box_vis(i_Nodes[j].m_pNode->GetWorldBox(), &proj_light->GetTotalMatrix(), true) == e_Reject)
							{
								continue;
							}
						}
					}
					g3dBlendStateMgr::SetBlendState(st_Blend);
					m_stats.m_nTriangles += g3dSceneRenderUtil::DrawNodeLit(i_Nodes[j].m_pNode, pLight, proj_light);
				}
			}
		}

		g3dLightMgrVk::Implementation()->DisableLight( pLight );
		g3dSingleLightRendering::SetActiveLight( NULL );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPassLit::LightingLoopPerNode(const std::vector<g3dLight*>& i_Lights, const nodeCacheList& i_Nodes, g3dRenderStateTraverser* i_RenderStateCache)
{
	int j, n;
	// draw opaques per-light, shaded, and add to hdr buffer
	n = i_Nodes.size();
	for (j = 0; j < n; j++)
	{
		m_stats.m_nTriangles += RenderNode(i_Nodes[j], i_Lights, i_RenderStateCache);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int shdwPassLit::RenderNode(const sNodePlusState& i_Node, const std::vector<g3dLight*>& i_Lights, g3dRenderStateTraverser* i_RenderStateCache)
{
	int i;
	int nTrianglesDrawn = 0;
	// draw opaques per-light, shaded, and add to hdr buffer
	for (i = 0; i < i_Lights.size(); i++)
	{
		g3dLight* pLight = i_Lights[i];
		if ( !pLight->IsEnabled() ) 
			continue;
		if ( !pLight->GetCastsShadow() ) 
			continue;

		const g3dProjectedLight* proj_light = dynamic_cast<const g3dProjectedLight*>(pLight);
		if (proj_light)
		{	
			if (!g3dPrefs::CurrentPrefs().m_bProjLightsOn)
				continue;
		}
		else if (!g3dPrefs::CurrentPrefs().m_bPtLightsOn)
			continue;


		g3dSingleLightRendering::SetActiveLight( pLight );
		g3dLightMgrVk::Implementation()->SetLight( pLight );
		g3dLightMgrVk::Implementation()->EnableLight( pLight );

		if (!i_Node.m_pNode->GetFragment()->IsShadowHull())
		{
			if (i_RenderStateCache->IsLightInState(i_Node.m_StateCache, pLight))
			{
				if (g3dPrefs::CurrentPrefs().m_bProjLightFrustumCull)
				{
					if (proj_light != NULL)
					{
						if (g3dSceneRenderUtil::get_box_vis(i_Node.m_pNode->GetWorldBox(), &proj_light->GetTotalMatrix(), true) == e_Reject)
						{
							continue;
						}
					}
				}
				g3dBlendStateMgr::SetBlendState(st_Blend);
				nTrianglesDrawn += g3dSceneRenderUtil::DrawNodeLit(i_Node.m_pNode, pLight, proj_light);
			}
		}


		g3dLightMgrVk::Implementation()->DisableLight( pLight );
		g3dSingleLightRendering::SetActiveLight( NULL );
	}

	return nTrianglesDrawn;
//	g3dLightMgrVk::Implementation()->DisableAllLights();
//
//	int nTrianglesDrawn = 0;
//
//	const g3dSceneNode* node = i_Node.m_pNode;
//	// quick out if node is designated as shadow hull.
//	if (node->GetFragment()->IsShadowHull())
//		return 0;
//
//	static std::vector<g3dLight*> ptLights;
//	std::vector<g3dLight*>::iterator iPtL;
//	static std::vector<g3dProjectedLight*> projLights;
//	std::vector<g3dProjectedLight*>::iterator iPjL;
//	int i;
//	// draw opaques per-light, shaded, and add to hdr buffer
//	g3dLight* light = NULL;
//	g3dProjectedLight* proj_light = NULL;
//
//	// gather lights for fragment, point and projected ones.
//	ptLights.clear();
//	projLights.clear();
//	for (i = 0; i < i_Lights.size(); i++)
//	{
//		light = i_Lights[i];
//		if ( !light->IsEnabled() ) 
//			continue;
//		if ( !light->GetCastsShadow() ) 
//			continue;
//		if (i_RenderStateCache->IsLightInState(i_Node.m_StateCache, light))
//		{
//			proj_light = dynamic_cast<g3dProjectedLight*>(light);
//			if (proj_light != NULL)
//			{
//				// if node can be culled (outside prjlt's frustum), then it won't be lit by that light.
//				if (g3dPrefs::CurrentPrefs().m_bProjLightFrustumCull)
//				{
//					if (g3dSceneRenderUtil::get_box_vis(node->GetWorldBox(), &proj_light->GetTotalMatrix(), true) == e_Reject)
//					{
//						continue;
//					}
//				}
//				// else add light to list.
//				projLights.push_back(proj_light);
//			}
//			else
//				ptLights.push_back(light);
//		}
//	}
//
//	// now the lights are in 2 lists. we can render now.
//	if (g3dPrefs::CurrentPrefs().m_bPtLightsOn)
//	{
//		int nPtL = ptLights.size();
//		for (i = 0; i < nPtL; ++i)
//		{
//			light = ptLights[i];
//			g3dSingleLightRendering::SetActiveLight( light );
//			g3dLightMgrVk::Implementation()->SetLight( light );
//			g3dLightMgrVk::Implementation()->EnableLight( light );
//
//
//			nTrianglesDrawn += g3dSceneRenderUtil::DrawNodeLit(node, light, NULL);
//			g3dLightMgrVk::Implementation()->DisableLight( light );
//			g3dSingleLightRendering::SetActiveLight( NULL );
//
//		}
//	}
//	if (g3dPrefs::CurrentPrefs().m_bProjLightsOn)
//	{
//		int nPjL = projLights.size();
//		for (i = 0; i < nPjL; ++i)
//		{
//			proj_light = projLights[i];
//			g3dSingleLightRendering::SetActiveLight( proj_light );
//			g3dLightMgrVk::Implementation()->SetLight( proj_light );
//			g3dLightMgrVk::Implementation()->EnableLight( proj_light );
//
//
//			nTrianglesDrawn += g3dSceneRenderUtil::DrawNodeLit(node, proj_light, proj_light);
//			g3dLightMgrVk::Implementation()->DisableLight( proj_light );
//			g3dSingleLightRendering::SetActiveLight( NULL );
//
//		}
//	}
//	return nTrianglesDrawn;
}

}
