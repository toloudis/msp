/*****************************************************************************
**	gpxGlobalAmbient.hpp
**
**	This class is a thread-safe proxy for the api3dScene´s global ambient state
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_GLOBALAMBIENT_HPP
#error gpxGlobalAmbient.hpp multiply included
#endif
#define GPX_GLOBALAMBIENT_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 

#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif 


//============================================================================
//============================================================================
class gpxGlobalAmbient : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	gpxGlobalAmbient();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxGlobalAmbient();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the character when "Update()" is called.
	//--------------------------------------------------------------------
	void SetGlobalAmbient(const g3dAmbientEnvState& i_GlobalAmbient);

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
#if USE_PROXIES
	g3dAmbientEnvState m_GlobalAmbient;
#endif
};
