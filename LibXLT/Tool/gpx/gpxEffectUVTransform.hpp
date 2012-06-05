/*****************************************************************************
**	gpxEffectUVTransform.hpp
**
**	This class is a thread-safe proxy for a effUVTransform.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_EFFECTUVTRANSFORM_HPP
#error gpxEffectUVTransform.hpp multiply included
#endif
#define GPX_EFFECTUVTRANSFORM_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 


//============================================================================
//	forward references
//============================================================================
class effUVTransform;


//============================================================================
//============================================================================
class gpxEffectUVTransform : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxEffectUVTransform(effUVTransform &i_Effect);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxEffectUVTransform();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the effect when "Update()" is called.
	//--------------------------------------------------------------------
	void SetUVTransform(float i_UScale, float i_VScale, 
						float i_UTrans, float i_VTrans, 
						float i_UVAngle);

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	effUVTransform &m_Effect;

#if USE_PROXIES
	float m_UScale, m_VScale, m_UTrans, m_VTrans, m_UVAngle;
#endif
};
