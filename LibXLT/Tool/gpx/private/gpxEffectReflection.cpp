/*****************************************************************************
**	gpxEffectReflection.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxEffectReflection.hpp"

#include "Graphics/Eff/effReflData.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxEffectReflection::gpxEffectReflection(effReflectionMap &i_Effect)
:	m_Effect(i_Effect)
{
#if USE_PROXIES
	m_NearPlane = i_Effect.m_NearPlane;
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxEffectReflection::~gpxEffectReflection()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxEffectReflection::SetNearPlane(float i_Near)
{
	PROXY_SET_VARIABLE(m_Effect.m_NearPlane, m_NearPlane, i_Near);
}
//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxEffectReflection::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	m_Effect.m_NearPlane = m_NearPlane;

	this->SetNeedsUpdate(false);
#endif

	return true;
}
