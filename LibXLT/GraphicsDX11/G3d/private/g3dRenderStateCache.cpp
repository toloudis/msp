/****************************************************************************\
**	g3dRenderStateCache.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g3d/g3dRenderStateCache.hpp"

#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
//#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"

#include <algorithm>

namespace
{	
	void get_render_state_from_light_mgr(g3dRenderState& i_RenderState)
	{
		i_RenderState.m_Lights = g3dLightMgrDX11::Implementation()->GetLights();
	}

}	// end of anonymous namespace


//------------------------------------------------------------------------
//------------------------------------------------------------------------
g3dRenderStateTraverser::g3dRenderStateTraverser()
: m_Depth(0), m_CurrentCache(0), 
  m_LastCache(0), m_pLastLight(NULL), m_bLastAnswer(false)
{
	// To allow code to work when no render states are set in the
	// graph, all lights should be on when starting the render.
	// We cache these into the m_FullRenderState. If we get a
	// render state, then the render states control what lights
	// are set (the full state is turned off). If we never get a
	// render state, then we use the full state for all nodes.
	//
	get_render_state_from_light_mgr(this->m_FullRenderState);
	
	// start with full lights as cached state index #0
	m_CachedStates.push_back(m_FullRenderState);
	m_CachedEnvStates.push_back(g3dAmbientEnvState());

	m_EnvStack.push(g3dAmbientEnvState());
}

//------------------------------------------------------------------------
//	AddRenderState - add to the render state
//		If i_bJustAmbientLights is true, shadow casting lights
//		are not enabled (they will get separate pass later)
//------------------------------------------------------------------------
void g3dRenderStateTraverser::AddRenderState( const g3dRenderState* i_pRenderState,
											 const g3dAmbientEnvState* i_pEnvironment,
											 bool i_bJustAmbientLights )
{
	if( i_pRenderState )
	{
		// At the first render state, turn off all of the lights so
		// that the render state defines the only lights that are on.
		if (m_Depth == 0)
		{
			g3dLightMgrDX11::Implementation()->DisableAllLights();
			g3dLightMgr::SetAmbient( maFloatRGBA(0,0,0,0) );
		}
		m_Depth++;

		// Don't allow contributions to the active lights if the headlight is on
		if (!g3dLightMgr::IsHeadlightEnabled())
		{
			int nSize = i_pRenderState->m_Lights.size();
			for( int i = 0; i < nSize; ++i )
			{
				g3dLight* pLight = i_pRenderState->m_Lights[i];
				if( pLight->IsEnabled() )
				{
					if (!i_bJustAmbientLights || !pLight->GetCastsShadow())
					{
						g3dLightMgrDX11::Implementation()->SetLight( pLight );
						g3dLightMgrDX11::Implementation()->EnableLight( pLight );
					}
					m_CurrentRenderState.m_Lights.push_back( pLight );
				}
			}
		}
	}
	if (i_pEnvironment)
	{
		m_EnvStack.push(*i_pEnvironment);
		m_CurrentEnvState = *i_pEnvironment;
	}
}

//------------------------------------------------------------------------
//	SubtractRenderState - subtracts from the render state
//		If i_bJustAmbientLights is true, shadow casting lights
//		are not enabled (they will get separate pass later)
//------------------------------------------------------------------------
void g3dRenderStateTraverser::SubtractRenderState( const g3dRenderState* i_pRenderState,
												  const g3dAmbientEnvState* i_pEnvironment,
												  bool i_bJustAmbientLights )
{
	if( i_pRenderState )
	{
		// Don't alter the active lights if the headlight is on
		if (!g3dLightMgr::IsHeadlightEnabled())
		{
			int nSize = i_pRenderState->m_Lights.size();
			for( int i = 0; i < nSize; ++i )
			{
				g3dLight* pLight = i_pRenderState->m_Lights[i];
				if( pLight->IsEnabled() )
				{
					if (!i_bJustAmbientLights || !pLight->GetCastsShadow())
					{
						g3dLightMgrDX11::Implementation()->DisableLight( pLight );
					}
					m_CurrentRenderState.m_Lights.pop_back();
				}
			}
		}

		m_Depth--;
		if (m_Depth == 0)
		{
			// Popped back to initial state
			set_render_state(this->m_FullRenderState, i_bJustAmbientLights);
		}
	}
	if (i_pEnvironment)
	{
		m_CurrentEnvState = m_EnvStack.top();
		m_EnvStack.pop();
	}

}

//------------------------------------------------------------------------
// GetCurrentStateCache - get handle to current render state in order
//	to set the render state again later during delayed rendering.
//------------------------------------------------------------------------
g3dRenderStateCache g3dRenderStateTraverser::GetCurrentStateCache()
{
	// Special case when no render states have been traversed, 
	// use the cache position 0 for the "all lights on" state.
	if (m_Depth == 0) return 0;

	// Look for existing state in our cache.
	// Go backwards because it is more likely to find a match 
	// at the end of the cache.
	for (int i = m_CachedStates.size()-1; i>=0; i--)
	{
		if ((m_CachedStates[i] == m_CurrentRenderState) && 
			(m_CachedEnvStates[i] == m_CurrentEnvState))
			return i;
	}

	// if not found, add the state at the end of the cache.
	m_CachedStates.push_back(m_CurrentRenderState);
	m_CachedEnvStates.push_back(m_CurrentEnvState);
	return (m_CachedStates.size()-1);
}

//------------------------------------------------------------------------
//	SetRenderState - forces the render state, used in delayed
//		node rendering
//------------------------------------------------------------------------
void g3dRenderStateTraverser::SetRenderState( g3dRenderStateCache i_Cache,
											  bool i_bJustAmbientLights ) const
{
	DBG_ASSERT(m_Depth == 0, "Setting a render state cache is not valid during traversal.");
	DBG_ASSERT(i_Cache < m_CachedStates.size(), "Cache index out of range");

	if (m_CurrentCache != i_Cache)
	{
		set_render_state( m_CachedStates[i_Cache], i_bJustAmbientLights );
		// also set env state here too?

		m_CurrentCache = i_Cache;
	}
}

//------------------------------------------------------------------------
// Returns true if the given light is in the cache
//------------------------------------------------------------------------
bool g3dRenderStateTraverser::IsLightInState( g3dRenderStateCache i_Cache, g3dLight *i_pLight ) const
{
	// special case index 0 as "all lights on"
	if (i_Cache==0) return true;
	
	// Checked last asked value to take advantage of coherence
	if (m_LastCache == i_Cache && m_pLastLight == i_pLight)
		return m_bLastAnswer;

	m_LastCache = i_Cache;
	m_pLastLight = i_pLight;

	DBG_ASSERT(m_Depth == 0, "Setting a render state cache is not valid during traversal.");
	DBG_ASSERT(i_Cache < m_CachedStates.size(), "Cache index out of range");

	const std::vector<g3dLight*>& lights = m_CachedStates[i_Cache].m_Lights;
	m_bLastAnswer = (std::find(lights.begin(), lights.end(), i_pLight) != lights.end());
	return m_bLastAnswer;
}

//------------------------------------------------------------------------
// Get RenderState for given cache, used for multi-pass rendering.
//------------------------------------------------------------------------
const g3dRenderState& g3dRenderStateTraverser::GetRenderState( g3dRenderStateCache i_Cache ) const
{
	DBG_ASSERT(i_Cache < m_CachedStates.size(), "Cache index out of range");
	return  m_CachedStates[i_Cache];
}
//------------------------------------------------------------------------
// Get AmbientEnvState for given cache, used for multi-pass rendering.
//------------------------------------------------------------------------
const g3dAmbientEnvState& g3dRenderStateTraverser::GetEnvironmentState( g3dRenderStateCache i_Cache ) const
{
	DBG_ASSERT(i_Cache < m_CachedEnvStates.size(), "Cache index out of range");
	return  m_CachedEnvStates[i_Cache];
}

//------------------------------------------------------------------------
//	set_render_state - forces the render state, used in delayed
//		node rendering
//		If i_bJustAmbientLights is true, shadow casting lights
//		are not enabled (they will get separate pass later)
//------------------------------------------------------------------------
void g3dRenderStateTraverser::set_render_state( const g3dRenderState& i_RenderState,
												bool i_bJustAmbientLights ) const
{
	g3dLightMgrDX11::Implementation()->DisableAllLights();
	//g3dLightMgr::SetAmbient( maFloatRGBA(0,0,0,0) );


	int nSize = i_RenderState.m_Lights.size();
	for( int i = 0; i < nSize; ++i )
	{
		g3dLight* pLight = i_RenderState.m_Lights[i];

		if( pLight->IsEnabled() )
		{
			if (!i_bJustAmbientLights || !pLight->GetCastsShadow())
			{
				g3dLightMgrDX11::Implementation()->SetLight( pLight );
				g3dLightMgrDX11::Implementation()->EnableLight( pLight );
			}
		}
	}
}
