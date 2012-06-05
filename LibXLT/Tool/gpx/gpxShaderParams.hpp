/*****************************************************************************
**	gpxShaderParams.hpp
**
**		This class is a thread-safe proxy for a effShaderParams.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_SHADERPARAMS_HPP
#error gpxShaderParams.hpp multiply included
#endif
#define GPX_SHADERPARAMS_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 
#ifndef PRTY_PROPERTYUIINFOCONTAINER_HPP
#include "Core/prty/prtyPropertyUIInfoContainer.hpp"
#endif 


//============================================================================
//	forward references
//============================================================================
class effShaderParams;


//============================================================================
//============================================================================
class gpxShaderParams : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxShaderParams(effShaderParams &i_Effect);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxShaderParams();

	//--------------------------------------------------------------------
	// Return shader params to use for UI. When proxied,
	// these UIInfos do not control the effShaderParams directly, they
	// are hooked up to a new set that buffers the changes.
	//--------------------------------------------------------------------
	effShaderParams& UIShaderParams();
	const effShaderParams& GetUIShaderParams() const;

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	effShaderParams &m_Effect;

#if USE_PROXIES
	// Copy of shader params to present to the UI and monitor for changes
	shared_ptr<effShaderParams> m_UIParams;
#endif

	//----------------------------------------------------------------------------
	// property callback
	//----------------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
};
