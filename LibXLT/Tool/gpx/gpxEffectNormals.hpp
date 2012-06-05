/*****************************************************************************
**	gpxEffectNormals.hpp
**
**	This class is a thread-safe proxy for a effNormalsData.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_EFFECTNORMALS_HPP
#error gpxEffectNormals.hpp multiply included
#endif
#define GPX_EFFECTNORMALS_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 


//============================================================================
//	forward references
//============================================================================
class effNormalsData;


//============================================================================
//============================================================================
class gpxEffectNormals : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxEffectNormals(effNormalsData &i_Effect);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxEffectNormals();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the effect when "Update()" is called.
	//--------------------------------------------------------------------
	void SetBumpScale(float i_BumpScale);

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	effNormalsData &m_Effect;

#if USE_PROXIES
	float m_BumpScale;
#endif
};
