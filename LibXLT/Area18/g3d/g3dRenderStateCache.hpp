/****************************************************************************\
**	g3dRenderStateCache.hpp
**
**	g3dRenderStateTraverser handles the push/pop/set of render states;
**	adjusting the light manager accordingly. 
**	g3dRenderStateCache remembers state at given point in traversal 
**	in order to set	it back again for delayed rendering.
**
** Area17
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef G3D_RENDERSTATECACHE_HPP
#error g3dRenderStateCache.hpp multiply included
#endif
#define G3D_RENDERSTATECACHE_HPP

#ifndef G3D_RENDERSTATE_HPP
#include "Graphics/g3d/g3dRenderState.hpp"
#endif

#include <stack>

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//	Typedefs
//--------------------------------------------------------------------
typedef int g3dRenderStateCache;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class  g3dRenderStateTraverser
{
public:
	//------------------------------------------------------------------------
	// If you want to allow scene graphs to not use render states, then
	// all default lights should be on during the constructor. If no
	// render states are found, these lights will be on for the whole scene.
	// If a render state is found, then the default lights are turned off
	// and only the lights in the render state are used.
	//------------------------------------------------------------------------
	g3dRenderStateTraverser();

	//------------------------------------------------------------------------
	//	AddRenderState - add to the render state.
	//		If i_bJustAmbientLights is true, shadow casting lights
	//		are not enabled (they will get separate pass later)
	//------------------------------------------------------------------------
	void AddRenderState( const g3dRenderState* i_pRenderState,
						const g3dAmbientEnvState* i_pEnvironment,
						 bool i_bJustAmbientLights = false );

	//------------------------------------------------------------------------
	//	SubtractRenderState - subtracts from the render state
	//		If i_bJustAmbientLights is true, shadow casting lights
	//		are not enabled (they will get separate pass later)
	//------------------------------------------------------------------------
	void SubtractRenderState( const g3dRenderState* i_pRenderState,
						const g3dAmbientEnvState* i_pEnvironment,
						 bool i_bJustAmbientLights = false );

	//------------------------------------------------------------------------
	// GetCurrentStateCache - get handle to current render state in order
	//	to set the render state again later during delayed rendering.
	//------------------------------------------------------------------------
	g3dRenderStateCache GetCurrentStateCache();

	//------------------------------------------------------------------------
	//	SetRenderState - forces the render state, used in delayed
	//		node rendering.
	//		If i_bJustAmbientLights is true, shadow casting lights
	//		are not enabled (they will get separate pass later)
	//------------------------------------------------------------------------
	void SetRenderState( g3dRenderStateCache i_Cache,
						 bool i_bJustAmbientLights = false ) const;

	//------------------------------------------------------------------------
	// Returns true if the given light is in the cache
	//------------------------------------------------------------------------
	bool IsLightInState( g3dRenderStateCache i_Cache, g3dLight *i_pLight ) const;

	//------------------------------------------------------------------------
	// Get RenderState for given cache, used for multi-pass rendering.
	//------------------------------------------------------------------------
	const g3dRenderState& GetRenderState( g3dRenderStateCache i_Cache ) const;

	//------------------------------------------------------------------------
	// Get AmbientEnvState for given cache, used for multi-pass rendering.
	//------------------------------------------------------------------------
	const g3dAmbientEnvState& GetEnvironmentState( g3dRenderStateCache i_Cache ) const;

private:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void set_render_state( const g3dRenderState& i_RenderState,
							bool i_bJustAmbientLights ) const;

	// Current render state during traversal
	g3dRenderState m_CurrentRenderState;

	// Current environment state during traversal
	g3dAmbientEnvState m_CurrentEnvState;

	// Start render state based on lights that were enabled at start
	g3dRenderState m_FullRenderState;

	// Depth of traversal, nonzero when in traversal
	int m_Depth;

	// Vector of cached states
	std::vector<g3dRenderState> m_CachedStates;
	std::vector<g3dAmbientEnvState> m_CachedEnvStates;

	// Current set state during delayed traversal
	mutable int m_CurrentCache;

	// Last asked value for IsLightInState to take advantage of coherence
	mutable g3dRenderStateCache m_LastCache;
	mutable g3dLight *m_pLastLight;
	mutable bool m_bLastAnswer;

	// keep a stack of ambient env states
	std::stack<g3dAmbientEnvState> m_EnvStack;
};
