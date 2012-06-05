/*****************************************************************************
**	gpxRenderState.hpp
**
**		This class is a thread-safe proxy for a g3dRenderState.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_RENDERSTATE_HPP
#error gpxRenderState.hpp multiply included
#endif
#define GPX_RENDERSTATE_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 
#ifndef G3D_RENDERSTATE_HPP
#include "Graphics/G3d/g3dRenderState.hpp"
#endif 


//============================================================================
//	forward references
//============================================================================
class g3dSceneNode;


//============================================================================
//============================================================================
class gpxRenderState : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxRenderState(g3dSceneNode &i_Object);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxRenderState();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the render state when "Update()" is called.
	//--------------------------------------------------------------------
	void AddLightToRenderState(g3dLight *i_pLight);
	void RemoveLightFromRenderState(g3dLight *i_pLight);

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	g3dSceneNode &m_SceneNode;
	g3dRenderState* m_pRenderState;

#if USE_PROXIES 
	std::vector<g3dLight*> m_Lights;
#endif
};
