/*****************************************************************************
**	gpxRenderState.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxRenderState.hpp"

#include "Core/Env/envSTLHelpers.hpp"
#include "Graphics/G3d/g3dSceneNode.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxRenderState::gpxRenderState(g3dSceneNode &i_SceneNode)
:	m_SceneNode(i_SceneNode), 
	m_pRenderState(NULL)
{
	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxRenderState::~gpxRenderState()
{
	PROXY_REMOVE();

	if (m_pRenderState)
	{
		delete m_pRenderState;
		m_SceneNode.SetRenderState(NULL);
	}
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the character when "Update()" is called.
//--------------------------------------------------------------------
void gpxRenderState::AddLightToRenderState(g3dLight *i_pLight)
{
#if USE_PROXIES
	m_Lights.push_back(i_pLight);
	this->SetNeedsUpdate(true);
#else
	if (!m_pRenderState)
	{
		m_pRenderState = new g3dRenderState();
		m_SceneNode.SetRenderState(m_pRenderState);
	}
	m_pRenderState->m_Lights.push_back(i_pLight);
#endif
}
void gpxRenderState::RemoveLightFromRenderState(g3dLight *i_pLight)
{
#if USE_PROXIES
	envSTLHelpers::RemoveOneValue(m_Lights, i_pLight);
	this->SetNeedsUpdate(true);
#else
	if (m_pRenderState)
	{
		envSTLHelpers::RemoveOneValue(m_pRenderState->m_Lights, i_pLight);
	}
#endif

}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxRenderState::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	if (m_Lights.empty())
	{
		// Special case of empty light list, make sure render state is empty
		if (m_pRenderState)
			m_pRenderState->m_Lights.clear();
	}
	else
	{
		// Create render state if needed
		if (!m_pRenderState)
		{
			m_pRenderState = new g3dRenderState();
			m_SceneNode.SetRenderState(m_pRenderState);
		}	

		// Assign our list of lights to the render state
		m_pRenderState->m_Lights = m_Lights;
	}

	this->SetNeedsUpdate(false);
#endif

	return true;
}
