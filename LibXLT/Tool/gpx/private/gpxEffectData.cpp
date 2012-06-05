/*****************************************************************************
**	gpxEffectData.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxEffectData.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor 
//--------------------------------------------------------------------
gpxEffectData::gpxEffectData()
{
	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxEffectData::~gpxEffectData()
{
	PROXY_REMOVE();
}

//--------------------------------------------------------------------
//	Source data has changed, update target now or on Update()
//--------------------------------------------------------------------
void gpxEffectData::SetEffectDataChanged()
{
#if USE_PROXIES
	this->SetNeedsUpdate(true);
#else
	this->UpdateTargetData();
#endif
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxEffectData::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	this->UpdateTargetData();

	this->SetNeedsUpdate(false);
#endif

	return true;
}
