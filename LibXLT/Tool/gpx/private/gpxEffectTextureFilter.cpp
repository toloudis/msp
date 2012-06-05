/*****************************************************************************
**	gpxEffectTextureFilter.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxEffectTextureFilter.hpp"

#include "Graphics/Eff/effTextureFilterData.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxEffectTextureFilter::gpxEffectTextureFilter(effTextureFilterData &i_Effect)
:	m_Effect(i_Effect)
{
#if USE_PROXIES
	m_bEnableMipmap = i_Effect.m_bEnableMipmap;
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxEffectTextureFilter::~gpxEffectTextureFilter()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxEffectTextureFilter::SetFilter(bool i_bEnableMipmap)
{
	PROXY_SET_VARIABLE(m_Effect.m_bEnableMipmap, m_bEnableMipmap, i_bEnableMipmap);
}
//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxEffectTextureFilter::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	m_Effect.m_bEnableMipmap = m_bEnableMipmap;

	this->SetNeedsUpdate(false);
#endif

	return true;
}
