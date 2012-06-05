/*****************************************************************************
**	gpxFog.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxFog.hpp"

#include "Tool/api3d/api3dScene.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxFog::gpxFog()
{
//#if USE_PROXIES
//	api3dScene::GetFogSettings(m_FogParams);
//#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxFog::~gpxFog()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the character when "Update()" is called.
//--------------------------------------------------------------------
void gpxFog::SetFog(const fogParams& i_FogParams)
{
#if USE_PROXIES
	m_FogParams = i_FogParams;
	this->SetNeedsUpdate(true);
#else
	api3dScene::SetFog(i_FogParams);
#endif
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxFog::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	api3dScene::SetFog(m_FogParams);

	this->SetNeedsUpdate(false);
#endif

	return true;
}
