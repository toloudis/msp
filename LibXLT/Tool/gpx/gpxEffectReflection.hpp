/*****************************************************************************
**	gpxEffectReflection.hpp
**
**	This class is a thread-safe proxy for a effReflectionMap.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_EFFECTREFLECTION_HPP
#error gpxEffectReflection.hpp multiply included
#endif
#define GPX_EFFECTREFLECTION_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 


//============================================================================
//	forward references
//============================================================================
class effReflectionMap;


//============================================================================
//============================================================================
class gpxEffectReflection : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxEffectReflection(effReflectionMap &i_Object);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxEffectReflection();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the effect when "Update()" is called.
	//--------------------------------------------------------------------
	void SetNearPlane(float i_Near);

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	effReflectionMap &m_Effect;

#if USE_PROXIES
	float m_NearPlane;
#endif
};
