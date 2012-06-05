/*****************************************************************************
**	gpxEffectNormals.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxEffectNormals.hpp"

#include "Graphics/Eff/effNormalsData.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxEffectNormals::gpxEffectNormals(effNormalsData &i_Effect)
:	m_Effect(i_Effect)
{
#if USE_PROXIES
	m_BumpScale = i_Effect.m_BumpScale;
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxEffectNormals::~gpxEffectNormals()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxEffectNormals::SetBumpScale(float i_BumpScale)
{
	PROXY_SET_VARIABLE(m_Effect.m_BumpScale, m_BumpScale, i_BumpScale);
}
//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxEffectNormals::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	m_Effect.m_BumpScale = m_BumpScale;

	this->SetNeedsUpdate(false);
#endif

	return true;
}
