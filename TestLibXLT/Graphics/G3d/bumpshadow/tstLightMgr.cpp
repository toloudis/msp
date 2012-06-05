/*****************************************************************************
**  tstLightMgr.cpp
**
**
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "tstLightMgr.hpp"

#include "g3dDirectionalLight.hpp"
#include "g3dPointLight.hpp"
#include "g3dLightManager.hpp"
#include "g3dSpotLight.hpp"
#include "g3dRenderState.hpp"
#include "g3dScene.hpp"
#include "g3dSceneNode.hpp"

#include <algorithm>

namespace
{
	g3dRenderState* l_pRootRenderState = NULL;
}

//====================================================================
// Initialize
//====================================================================
void tstLightMgr::Initialize()
{
	l_pRootRenderState = new g3dRenderState();
	l_pRootRenderState->m_AmbientLight.Set( 0.5f, 0.5f, 0.5f, 1.0f );

	g3dSceneNode* pRoot = g3dScene::GetWorldRoot();
	pRoot->SetRenderState( l_pRootRenderState );
}

//====================================================================
// DeInitialize
//====================================================================
void tstLightMgr::DeInitialize()
{
	delete l_pRootRenderState;
	l_pRootRenderState = NULL;

	g3dSceneNode* pRoot = g3dScene::GetWorldRoot();
	pRoot->SetRenderState( NULL );
}

//========================================================================
//	CreatePointLight
//========================================================================
g3dPointLight* tstLightMgr::CreatePointLight()
{
	DBG_ASSERT0( l_pRootRenderState, "tstLightMgr not initialized" );
	g3dPointLight* pLight = g3dLightManager::CreatePointLight();
	pLight->Enable();
	l_pRootRenderState->m_Lights.push_back( pLight );

	return pLight;
}

//========================================================================
//	CreateDirectionalLight
//========================================================================
g3dDirectionalLight* tstLightMgr::CreateDirectionalLight()
{
	DBG_ASSERT0( l_pRootRenderState, "tstLightMgr not initialized" );
	g3dDirectionalLight* pLight = g3dLightManager::CreateDirectionalLight();
	pLight->Enable();
	l_pRootRenderState->m_Lights.push_back( pLight );

	return pLight;
}

//========================================================================
//	CreateSpotLight
//========================================================================
g3dSpotLight* tstLightMgr::CreateSpotLight()
{
	DBG_ASSERT0( l_pRootRenderState, "tstLightMgr not initialized" );
	g3dSpotLight* pLight = g3dLightManager::CreateSpotLight();
	pLight->Enable();
	l_pRootRenderState->m_Lights.push_back( pLight );

	return pLight;
}

//========================================================================
//	DestroyLight
//========================================================================
void tstLightMgr::DestroyLight( g3dLight* i_pLight )
{
	g3dRenderState::Lights::iterator it = std::find(
												l_pRootRenderState->m_Lights.begin(),
												l_pRootRenderState->m_Lights.end(),
												i_pLight );

	DBG_ASSERT0( it != l_pRootRenderState->m_Lights.end(), "Light not found" );

	l_pRootRenderState->m_Lights.erase( it );
	delete i_pLight;
}