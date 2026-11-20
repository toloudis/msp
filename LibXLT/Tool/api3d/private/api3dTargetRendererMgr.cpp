/*****************************************************************************
**	api3dTargetRendererMgr.cpp
**
**		api3dTargetRendererMgr maintains targets for textures that need
**	to be updated by pre-renders of the scene.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dTargetRendererMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/g3d/g3dTargetRenderer.hpp"
#include "Graphics/G3d/g3dThreadControl.hpp"
#include "Tool/api3d/api3dScene.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace api3dTargetRendererMgr
{
	namespace
	{
		std::vector<g3dTargetRenderer*> l_ShadowTargetRenderers;
		std::vector<g3dTargetRenderer*> l_ReflectionTargetRenderers;
		std::vector<g3dTargetRenderer*> l_TextureRenderers;
		std::vector<g3dTargetRenderer*> l_OpacityTargetRenderers;
	}

	//------------------------------------------------------------------------
	// Add TargetRenderer to management
	//------------------------------------------------------------------------
	void AddTargetRenderer( g3dTargetRenderer *i_pTargetRenderer, 
		TargetUsage i_Usage /*= e_Shadow*/ )
	{
		if (i_Usage == e_Shadow)
			l_ShadowTargetRenderers.push_back(i_pTargetRenderer);
		else if (i_Usage == e_Reflection)
			l_ReflectionTargetRenderers.push_back(i_pTargetRenderer);
		else if (i_Usage == e_Texture)
			l_TextureRenderers.push_back(i_pTargetRenderer);
		else if (i_Usage == e_Opacity)
			l_OpacityTargetRenderers.push_back(i_pTargetRenderer);
		else
			DBG_ASSERT(false, "Bad target renderer usage flag");
	}

	//------------------------------------------------------------------------
	// Remove TargetRenderer from management
	//------------------------------------------------------------------------
	bool RemoveTargetRenderer( g3dTargetRenderer *i_pTargetRenderer )
	{
		bool removed = envSTLHelpers::RemoveOneValue(l_ShadowTargetRenderers, i_pTargetRenderer);
		if (!removed)
            removed = envSTLHelpers::RemoveOneValue(l_ReflectionTargetRenderers, i_pTargetRenderer);
		if (!removed)
			removed = envSTLHelpers::RemoveOneValue(l_TextureRenderers, i_pTargetRenderer);
		if (!removed)
			removed = envSTLHelpers::RemoveOneValue(l_OpacityTargetRenderers, i_pTargetRenderer);
		return removed;
	}

	//------------------------------------------------------------------------
	// Examine if the target renderer exists
	//------------------------------------------------------------------------
	bool IsTargetRendererExist( g3dTargetRenderer *i_pTargetRenderer )
	{
		bool existed = envSTLHelpers::Contains(l_ShadowTargetRenderers, i_pTargetRenderer);
		if (!existed)
            existed = envSTLHelpers::Contains(l_ReflectionTargetRenderers, i_pTargetRenderer);
		if (!existed)
			existed = envSTLHelpers::Contains(l_TextureRenderers, i_pTargetRenderer);
		if (!existed)
			existed = envSTLHelpers::Contains(l_OpacityTargetRenderers, i_pTargetRenderer);
		return existed;
	}

	//------------------------------------------------------------------------
	// Render all targets
	//------------------------------------------------------------------------
	void RenderTargets(float i_Time)
	{
		// no need to call Present() for these target renderers, since they are not double buffered

		// compute AO if needed.
		//if ((g3dPrefs::CurrentPrefs().m_RecalcAORequested) || 
		//	(g3dPrefs::CurrentPrefs().m_bRecalcAOPerFrame))
		//{
		//	api3dAmbientOcclusion::ComputeAmbientOcclusion(api3dScene::GetRoot(api3dScene::WorldLayerIndex()));
		//} 

		int nTargets = l_ShadowTargetRenderers.size();
		// only need to render shadow targets if shadows are on!
		// also, AO renderer doesn't need shadows
		// also, GI renderer doesn't need shadows
		if (g3dSingleLightRendering::GetDoSingleLightRendering() && 
			g3dPrefs::CurrentPrefs().m_bDoShadowMapGen &&
			g3dPrefs::CurrentPrefs().m_bEnableShadows &&
			(g3dPrefs::CurrentPrefs().m_RendererType != g3dSceneRendererTypes::e_AmbientOcclusion) &&
			(g3dPrefs::CurrentPrefs().m_RendererType != g3dSceneRendererTypes::e_GlobalIllumination)) 
		{
			for (int i=0; i<nTargets; i++)
			{
				// See if we should abort the render thread before doing the next target
				if (g3dThreadControl::ShouldRenderThreadAbort())
					break; // could return false here to signify that render did not finish correctly

				l_ShadowTargetRenderers[i]->Render(i_Time);
			}
		}

		// AO renderer doesn't need reflections.
		// GI renderer doesn't need reflections.
		if ((g3dPrefs::CurrentPrefs().m_RendererType != g3dSceneRendererTypes::e_AmbientOcclusion) &&
			(g3dPrefs::CurrentPrefs().m_RendererType != g3dSceneRendererTypes::e_GlobalIllumination) &&
			g3dPrefs::CurrentPrefs().m_bEnableReflection)
		{
			nTargets = l_ReflectionTargetRenderers.size();
			for (int i=0; i<nTargets; i++)
			{
				// See if we should abort the render thread before doing the next target
				if (g3dThreadControl::ShouldRenderThreadAbort())
					break; // could return false here to signify that render did not finish correctly

				l_ReflectionTargetRenderers[i]->Render(i_Time);
			}
		}

		// Render textures
		{
			nTargets = l_TextureRenderers.size();
			for (int i=0; i<nTargets; i++)
			{
				// See if we should abort the render thread before doing the next target
				if (g3dThreadControl::ShouldRenderThreadAbort())
					break; // could return false here to signify that render did not finish correctly
				
				
				l_TextureRenderers[i]->Render(i_Time);
			}

			// texture only need to be rendered once
			l_TextureRenderers.clear();
		}


		// only need to render Opacity targets if shadows and hair are on!
		// also, AO renderer doesn't need shadows
		// also, GI renderer doesn't need shadows
		if (g3dSingleLightRendering::GetDoSingleLightRendering() && 
			g3dPrefs::CurrentPrefs().m_bDoShadowMapGen &&
			g3dPrefs::CurrentPrefs().m_bEnableShadows &&
			g3dPrefs::CurrentPrefs().m_bEnableHair &&
			(g3dPrefs::CurrentPrefs().m_RendererType != g3dSceneRendererTypes::e_AmbientOcclusion) &&
			(g3dPrefs::CurrentPrefs().m_RendererType != g3dSceneRendererTypes::e_GlobalIllumination)) 
		{
			nTargets = l_OpacityTargetRenderers.size();
			for (int i=0; i<nTargets; i++)
			{
				// See if we should abort the render thread before doing the next target
				if (g3dThreadControl::ShouldRenderThreadAbort())
					break; // could return false here to signify that render did not finish correctly

				l_OpacityTargetRenderers[i]->Render(i_Time);
			}
		}

	}
};