/*****************************************************************************
**	gpxEffectTextureFilter.hpp
**
**	This class is a thread-safe proxy for a effTextureFilter.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_EFFECTTEXTUREFILTER_HPP
#error gpxEffectTextureFilter.hpp multiply included
#endif
#define GPX_EFFECTTEXTUREFILTER_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif

//============================================================================
//	forward references
//============================================================================
class effTextureFilterData;


//============================================================================
//============================================================================
class gpxEffectTextureFilter : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxEffectTextureFilter(effTextureFilterData &i_Effect);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxEffectTextureFilter();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the effect when "Update()" is called.
	//--------------------------------------------------------------------
	void SetFilter(bool i_bEnableMipmap);

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	effTextureFilterData &m_Effect;

#if USE_PROXIES
	bool m_bEnableMipmap;
#endif
};
