/*****************************************************************************
**	gpxGlobalIllumination.hpp
**
**	This class is a thread-safe proxy for the api3dScene´s ssgiParams
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_GLOBALILLUMINATION_HPP
#error gpxGlobalIllumination.hpp multiply included
#endif
#define GPX_GLOBALILLUMINATION_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 

#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif 


//============================================================================
//============================================================================
class gpxGlobalIllumination : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	gpxGlobalIllumination();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxGlobalIllumination();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the character when "Update()" is called.
	//--------------------------------------------------------------------
	void SetSSGI(const ssgiParams& i_SSGIParams);

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
#if USE_PROXIES
	ssgiParams m_GIParams;
#endif
};
