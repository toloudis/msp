/*****************************************************************************
**	api3dLightMgr.cpp
**
**	Creates and owns lights
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dLightMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dPointLight.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dScene.hpp"


//============================================================================
//============================================================================
namespace api3dLightMgr
{
	namespace
	{
		std::vector<g3dLight*> l_Lights;
		//g3dRenderState* l_pRootRenderState = NULL;

	}	// end of namespace


	//--------------------------------------------------------------------
	// Initialize
	//--------------------------------------------------------------------
	void Initialize()
	{
		//l_pRootRenderState = new g3dRenderState();

		// need to configure this somehow:
		//l_pRootRenderState->m_AmbientLight.Set( 0.5f, 0.5f, 0.5f, 1.0f );

		//g3dSceneNode* pRoot = g3dScene::GetWorldRoot();
		//pRoot->SetRenderState( l_pRootRenderState );
	}

	//--------------------------------------------------------------------
	// DeInitialize
	//--------------------------------------------------------------------
	void DeInitialize()
	{
		Clear();

		//delete l_pRootRenderState;
		//l_pRootRenderState = NULL;

		//g3dSceneNode* pRoot = g3dScene::GetWorldRoot();
		//pRoot->SetRenderState( NULL );
	}

	//--------------------------------------------------------------------
	//  Destroy all lights
	//--------------------------------------------------------------------
	void  Clear()
	{
		//if (l_pRootRenderState)
		//	envSTLHelpers::DeleteContainer(l_pRootRenderState->m_Lights);
		envSTLHelpers::ForAll(l_Lights, g3dLightMgr::DestroyLight);
		l_Lights.clear();
	}

	//--------------------------------------------------------------------
	//  Create new point light
	//--------------------------------------------------------------------
	g3dPointLight * CreatePointLight()
	{
		//DBG_ASSERT( l_pRootRenderState, "mtLightMgr not initialized" );
		g3dPointLight* pLight = g3dLightMgr::CreatePointLight();
		pLight->Enable();
		//l_pRootRenderState->m_Lights.push_back( pLight );
		l_Lights.push_back(pLight);

		return pLight;

	}

	//--------------------------------------------------------------------
	//  Create new directional light
	//--------------------------------------------------------------------
	g3dDirectionalLight * CreateDirectionalLight()
	{
		//DBG_ASSERT( l_pRootRenderState, "mtLightMgr not initialized" );
		g3dDirectionalLight* pLight = g3dLightMgr::CreateDirectionalLight();
		pLight->Enable();
		//l_pRootRenderState->m_Lights.push_back( pLight );
		l_Lights.push_back(pLight);

		return pLight;

	}

	//--------------------------------------------------------------------
	//  Create new projected light
	//--------------------------------------------------------------------
	g3dProjectedLight * CreateProjectedLight()
	{
		//DBG_ASSERT( l_pRootRenderState, "mtLightMgr not initialized" );
		g3dProjectedLight* pLight = g3dLightMgr::CreateProjectedLight();
		pLight->Enable();
		//l_pRootRenderState->m_Lights.push_back( pLight );
		l_Lights.push_back(pLight);

		return pLight;
	}

	//--------------------------------------------------------------------
	//  Destroy light, point or directional
	//--------------------------------------------------------------------
	void  DestroyLight(g3dLight *i_pLight)
	{
		g3dLightMgr::DestroyLight(i_pLight);
		envSTLHelpers::RemoveOneValue(l_Lights, i_pLight);
	}


}	// end of namespace
