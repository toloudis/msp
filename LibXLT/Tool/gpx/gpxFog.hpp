/*****************************************************************************
**	gpxFog.hpp
**
**	This class is a thread-safe proxy for the api3dScene´s fogParams
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_FOG_HPP
#error gpxFog.hpp multiply included
#endif
#define GPX_FOG_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 

#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif 


//============================================================================
//============================================================================
class gpxFog : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	gpxFog();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxFog();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the character when "Update()" is called.
	//--------------------------------------------------------------------
	void SetFog(const fogParams& i_FogParams);

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
#if USE_PROXIES
	fogParams m_FogParams;
#endif
};
