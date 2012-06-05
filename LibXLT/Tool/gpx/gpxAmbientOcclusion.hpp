/*****************************************************************************
**	gpxAmbientOcclusion.hpp
**
**	This class is a thread-safe proxy for the api3dScene´s ssaoParams
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_AMBIENTOCCLUSION_HPP
#error gpxAmbientOcclusion.hpp multiply included
#endif
#define GPX_AMBIENTOCCLUSION_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 

#ifndef G3D_SCENE_HPP
#include "Graphics/G3d/g3dScene.hpp"
#endif 


//============================================================================
//============================================================================
class gpxAmbientOcclusion : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	gpxAmbientOcclusion();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxAmbientOcclusion();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the character when "Update()" is called.
	//--------------------------------------------------------------------
	void SetSSAO(const ssaoParams& i_SSAOParams);

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
#if USE_PROXIES
	ssaoParams m_AOParams;
#endif
};
